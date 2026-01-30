
import os
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from std_msgs.msg import Float64MultiArray
from sensor_msgs.msg import JointState
import cv2
import numpy as np
import pandas as pd
from datetime import datetime
from typing import Dict, Any, Optional
import threading
import json
import time


class DataCollector(Node):
    def __init__(self, save_dir, chunk_id: int, episode_id: int = 0):
        super().__init__('data_collector')
        
        self.save_dir = save_dir
        self.chunk_id = chunk_id
        self.episode_id = episode_id
        
        # Create directory structure
        self.meta_dir = os.path.join(save_dir, "meta")
        self.videos_dir = os.path.join(save_dir, "videos", f"chunk-{chunk_id:03d}", "observation.images.ego_view")
        self.data_dir = os.path.join(save_dir, "data", f"chunk-{chunk_id:03d}")
        
        os.makedirs(self.meta_dir, exist_ok=True)
        os.makedirs(self.videos_dir, exist_ok=True)
        os.makedirs(self.data_dir, exist_ok=True)
        
        # Data storage
        self.data_rows = []
        
        # Current state
        self.joint_states = None
        self.target_joint_states = None
        self.latest_frame = None  # Store only the latest frame
        
        # Video writer
        self.video_writer = None
        self.video_fps = 15  # Match actual camera fps
        self.frame_count = 0
        self.last_written_index = -1  # Track last written frame index
        self.video_initialized = False
        
        # Video writer thread control
        self.writer_thread = None
        self.writer_running = False
        self.writer_lock = threading.Lock()
        
        # Episode info
        self.episode_start_time = None
        self.task_description = ""
        
        # Lock for thread safety
        self.lock = threading.Lock()
        
        # ROS2 subscribers
        self.joint_states_sub = self.create_subscription(
            JointState,
            '/igris/joint_states',
            self.joint_states_callback,
            10
        )
        
        self.target_joint_states_sub = self.create_subscription(
            JointState,
            '/igris/target_joint_states',
            self.target_joint_states_callback,
            10
        )
        
        self.image_sub = self.create_subscription(
            Image,
            '/camera/camera/color/image_raw',
            self.image_callback,
            10
        )
        
        self.get_logger().info(f"DataCollector initialized. Chunk: {chunk_id:03d}, Episode: {episode_id:06d}")
    
    def joint_states_callback(self, msg):
        with self.lock:
            self.joint_states = list(msg.position)
    
    def target_joint_states_callback(self, msg):
        with self.lock:
            self.target_joint_states = list(msg.position)
    
    def image_callback(self, msg):
        """
        Lightweight callback: only convert ROS Image to numpy array.
        NO VideoWriter.write() here to prevent blocking!
        """
        # Convert ROS Image message to OpenCV image without cv_bridge
        if msg.encoding == 'rgb8':
            dtype = np.uint8
            channels = 3
        elif msg.encoding == 'bgr8':
            dtype = np.uint8
            channels = 3
        elif msg.encoding == 'mono8':
            dtype = np.uint8
            channels = 1
        else:
            # Unsupported encoding, skip
            return
        
        # Convert to numpy array
        if channels > 1:
            img_array = np.frombuffer(msg.data, dtype=dtype).reshape(msg.height, msg.width, channels)
        else:
            img_array = np.frombuffer(msg.data, dtype=dtype).reshape(msg.height, msg.width)
        
        # Convert RGB to BGR if needed (OpenCV uses BGR)
        if msg.encoding == 'rgb8':
            cv_image = cv2.cvtColor(img_array, cv2.COLOR_RGB2BGR)
        else:
            cv_image = img_array
        
        # Just store the latest frame (overwrite)
        with self.writer_lock:
            self.latest_frame = cv_image.copy()
            
            # Initialize video writer on first frame (but don't write yet)
            if not self.video_initialized:
                height, width = cv_image.shape[:2]
                video_path = os.path.join(self.videos_dir, f"episode_{self.episode_id:06d}.mp4")
                fourcc = cv2.VideoWriter_fourcc(*'mp4v')
                self.video_writer = cv2.VideoWriter(video_path, fourcc, self.video_fps, (width, height))
                self.video_initialized = True
                self.get_logger().info(f"Video writer initialized: {video_path} ({width}x{height} @ {self.video_fps}fps)")
    
    def video_writer_thread_func(self):
        """
        Separate thread that periodically writes frames to mp4.
        Uses perf_counter and deadline-based drift correction.
        """
        target_interval = 1.0 / self.video_fps
        
        self.get_logger().info(f"Video writer thread started (target: {self.video_fps}fps)")
        
        # Use perf_counter for accurate timing
        next_deadline = time.perf_counter() + target_interval
        
        while self.writer_running:
            current_time = time.perf_counter()
            
            # Check if we've reached the deadline
            if current_time >= next_deadline:
                with self.writer_lock:
                    if self.latest_frame is not None and self.video_writer is not None:
                        # Write the latest frame
                        self.video_writer.write(self.latest_frame)
                        self.frame_count += 1
                        self.last_written_index = self.frame_count - 1
                
                # Update deadline (drift correction)
                next_deadline += target_interval
                
                # Prevent deadline drift if we're falling behind
                if next_deadline < current_time:
                    next_deadline = current_time + target_interval
            else:
                # Adaptive sleep to avoid busy-wait
                sleep_time = min(0.001, (next_deadline - current_time) * 0.5)
                time.sleep(sleep_time)
        
        self.get_logger().info(f"Video writer thread stopped. Total frames: {self.frame_count}")
    
    def start_video_writer_thread(self):
        """Start the video writer thread"""
        self.writer_running = True
        self.writer_thread = threading.Thread(target=self.video_writer_thread_func, daemon=True)
        self.writer_thread.start()
    
    def stop_video_writer_thread(self):
        """Stop the video writer thread"""
        self.writer_running = False
        if self.writer_thread is not None:
            self.writer_thread.join(timeout=2.0)
    
    def warmup_sensors(self, timeout_sec: float = 2.0) -> bool:
        """
        Wait for initial sensor data before starting episode.
        Returns True if all sensors ready, False if timeout.
        """
        self.get_logger().info(f"Warming up sensors (timeout: {timeout_sec}s)...")
        
        start_time = time.time()
        
        while time.time() - start_time < timeout_sec:
            # Spin to process callbacks
            rclpy.spin_once(self, timeout_sec=0.01)
            
            # Check if all sensors have data
            with self.lock:
                joints_ready = self.joint_states is not None
                target_ready = self.target_joint_states is not None
            
            with self.writer_lock:
                frame_ready = self.latest_frame is not None
            
            if joints_ready and target_ready and frame_ready:
                self.get_logger().info("All sensors ready!")
                return True
            
            time.sleep(0.01)
        
        # Timeout - log warning but continue
        self.get_logger().warn(
            f"Sensor warmup timeout. "
            f"joints={self.joint_states is not None}, "
            f"target={self.target_joint_states is not None}, "
            f"frame={self.latest_frame is not None}"
        )
        return False
    
    def append(self, row: Dict[str, Any]) -> None:
        """Append a data row to the collection."""
        with self.lock:
            self.data_rows.append(row)
    
    def collect_current_state(self, done: bool = False) -> Optional[Dict[str, Any]]:
        """
        Collect current state from all sensors and return as a dictionary.
        Returns None if data is not yet available.
        """
        with self.lock:
            if self.joint_states is None or self.target_joint_states is None:
                return None
            
            timestamp = self.get_clock().now().nanoseconds / 1e9
            
            if self.episode_start_time is None:
                self.episode_start_time = timestamp
            
            # Read last written frame index with lock
            with self.writer_lock:
                video_frame_idx = self.last_written_index
            
            row = {
                "timestamp": timestamp,
                "episode_index": self.episode_id,
                "frame_index": self.frame_count - 1 if self.frame_count > 0 else 0,
                "video_frame_index": video_frame_idx,  # Actual written frame index
                "observation.state": self.joint_states.copy(),
                "action": self.target_joint_states.copy(),
                "done": done,
            }
            
            return row
    
    def collect_episode(self, task: str = "", duration_sec: float = 30.0, rate_hz: float = 10.0):
        """
        Collect data for a fixed duration.
        
        Args:
            task: Task description
            duration_sec: Duration in seconds to collect data
            rate_hz: Sampling rate in Hz for joint/action data
        """
        self.task_description = task
        
        # Warmup: wait for sensors before starting
        self.warmup_sensors(timeout_sec=2.0)
        
        # Set start time AFTER warmup
        self.episode_start_time = self.get_clock().now().nanoseconds / 1e9
        start_time = time.time()
        sleep_duration = 1.0 / rate_hz
        
        self.get_logger().info(f"Starting episode {self.episode_id} (chunk {self.chunk_id:03d}) for {duration_sec}s. Task: '{task}'")
        
        # Start video writer thread
        self.start_video_writer_thread()
        
        try:
            while rclpy.ok():
                elapsed = time.time() - start_time
                
                # Check if duration has elapsed
                if elapsed >= duration_sec:
                    self.get_logger().info(f"Episode {self.episode_id} duration reached ({duration_sec}s)")
                    break
                
                # Spin once to process callbacks
                rclpy.spin_once(self, timeout_sec=0.001)
                
                # Collect joint/action data at specified rate (10Hz)
                row = self.collect_current_state(done=False)
                if row is not None:
                    self.append(row)
                
                time.sleep(sleep_duration)
                
        except KeyboardInterrupt:
            pass
        finally:
            # Stop video writer thread
            self.stop_video_writer_thread()
            
            # Mark last frame as done
            if len(self.data_rows) > 0:
                self.data_rows[-1]["done"] = True
            
            self.save_episode()
    
    def save_episode(self):
        """Save episode data to parquet file and finalize video."""
        self.get_logger().info(f"Saving episode {self.episode_id} (chunk {self.chunk_id:03d})...")
        
        with self.lock:
            # Save parquet file
            if len(self.data_rows) > 0:
                parquet_path = os.path.join(self.data_dir, f"episode_{self.episode_id:06d}.parquet")
                df = pd.DataFrame(self.data_rows)
                df.to_parquet(parquet_path, index=False)
                self.get_logger().info(f"Saved {len(self.data_rows)} rows to {parquet_path}")
            else:
                self.get_logger().warn("No data rows to save")
        
        # Finalize video (outside lock to avoid deadlock)
        with self.writer_lock:
            if self.video_writer is not None:
                self.video_writer.release()
                self.get_logger().info(f"Video saved with {self.frame_count} frames")
        
        # Update episodes.jsonl
        self._update_episodes_metadata()
        
        self.get_logger().info(f"Episode {self.episode_id} saved!")
    
    def _update_episodes_metadata(self):
        """Update episodes.jsonl with current episode info"""
        episodes_path = os.path.join(self.meta_dir, "episodes.jsonl")
        
        episode_info = {
            "episode_index": self.episode_id,
            "chunk_index": self.chunk_id,
            "tasks": [self.task_description] if self.task_description else [],
            "length": len(self.data_rows),
            "video_path": f"videos/chunk-{self.chunk_id:03d}/observation.images.ego_view/episode_{self.episode_id:06d}.mp4",
            "data_path": f"data/chunk-{self.chunk_id:03d}/episode_{self.episode_id:06d}.parquet",
        }
        
        # Append to jsonl file
        with open(episodes_path, 'a') as f:
            f.write(json.dumps(episode_info) + '\n')
    
    def __del__(self):
        """Cleanup video writer on deletion"""
        self.stop_video_writer_thread()
        with self.writer_lock:
            if self.video_writer is not None:
                self.video_writer.release()


def get_next_chunk_id(save_dir: str) -> int:
    """Get the next chunk ID by reading from meta/chunk_counter.txt"""
    meta_dir = os.path.join(save_dir, "meta")
    os.makedirs(meta_dir, exist_ok=True)
    
    counter_path = os.path.join(meta_dir, "chunk_counter.txt")
    
    if os.path.exists(counter_path):
        with open(counter_path, 'r') as f:
            chunk_id = int(f.read().strip())
    else:
        chunk_id = 0
    
    # Increment and save
    with open(counter_path, 'w') as f:
        f.write(str(chunk_id + 1))
    
    return chunk_id


def initialize_metadata(save_dir: str):
    """Initialize metadata files for LeRobot format"""
    meta_dir = os.path.join(save_dir, "meta")
    os.makedirs(meta_dir, exist_ok=True)
    
    # Create info.json
    info_path = os.path.join(meta_dir, "info.json")
    if not os.path.exists(info_path):
        info = {
            "fps": 15,  # Updated to match camera fps
            "robot_type": "igris",
            "created_at": datetime.now().isoformat(),
        }
        with open(info_path, 'w') as f:
            json.dump(info, f, indent=2)
    
    # Create modality.json (GR00T LeRobot specific)
    modality_path = os.path.join(meta_dir, "modality.json")
    if not os.path.exists(modality_path):
        modality = {
            "observation.state": {
                "dtype": "float32",
                "shape": [-1],
                "names": ["joint_positions"]
            },
            "observation.images.ego_view": {
                "dtype": "video",
                "shape": [480, 640, 3],
                "names": ["height", "width", "channels"]
            },
            "action": {
                "dtype": "float32", 
                "shape": [-1],
                "names": ["target_joint_positions"]
            }
        }
        with open(modality_path, 'w') as f:
            json.dump(modality, f, indent=2)
    
    # Create tasks.jsonl
    tasks_path = os.path.join(meta_dir, "tasks.jsonl")
    if not os.path.exists(tasks_path):
        open(tasks_path, 'w').close()


def main(args=None):
    rclpy.init(args=args)
    
    # Configuration
    base_dir = "robot_dataset"
    episode_duration = 5.0  # seconds per episode
    num_episodes = 5  # number of episodes to collect
    sampling_rate = 10.0  # Hz for joint/action data
    
    # Initialize metadata
    initialize_metadata(base_dir)
    
    # Get next chunk ID (increments each run)
    chunk_id = get_next_chunk_id(base_dir)
    
    print("=" * 60)
    print(f"Starting data collection:")
    print(f"  Chunk ID: {chunk_id:03d}")
    print(f"  Episodes: {num_episodes}")
    print(f"  Duration per episode: {episode_duration}s")
    print(f"  Joint/Action sampling rate: {sampling_rate}Hz")
    print(f"  Video fps: 15Hz (matched to camera)")
    print("=" * 60)
    
    # Collect multiple episodes
    for episode_id in range(num_episodes):
        try:
            # Optional: prompt for task description
            task = input(f"\nEnter task for episode {episode_id} (or press Enter to skip): ").strip()
            if not task:
                task = f"Task {episode_id}"
            
            # Create collector for this episode
            collector = DataCollector(base_dir, chunk_id=chunk_id, episode_id=episode_id)
            
            # Collect data for fixed duration
            collector.collect_episode(task=task, duration_sec=episode_duration, rate_hz=sampling_rate)
            
            # Cleanup
            collector.destroy_node()
            
            print(f"Episode {episode_id}/{num_episodes} completed\n")
            
            # Small delay between episodes
            time.sleep(1.0)
            
        except KeyboardInterrupt:
            print("\nCollection interrupted by user")
            break
        except Exception as e:
            print(f"Error in episode {episode_id}: {e}")
            import traceback
            traceback.print_exc()
            break
    
    print("=" * 60)
    print(f"Data collection complete! Chunk {chunk_id:03d} saved.")
    print(f"Total episodes collected: {episode_id + 1}/{num_episodes}")
    print("=" * 60)
    
    rclpy.shutdown()


if __name__ == '__main__':
    main()