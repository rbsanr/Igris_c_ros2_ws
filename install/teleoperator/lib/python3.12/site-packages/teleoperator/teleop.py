#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_srvs.srv import SetBool
from interface.srv import MoveJoints

from dynamixel_sdk import *
import threading
import time
import math

# -------------------------------------------------------------------
# [설정] 로봇 파라미터 & DXL 주소
# -------------------------------------------------------------------
# robot_params = [
#     {"name": "Waist_Yaw",           "ID": 0,   "Publish": 0, "dir": 1},
#     {"name": "Waist_Roll",          "ID": 1,   "Publish": 0, "dir": 1},
#     {"name": "Waist_Pitch",         "ID": 2,   "Publish": 0, "dir": 1},
#     {"name": "Hip_Pitch_L",         "ID": 3,   "Publish": 0, "dir": 1},
#     {"name": "Hip_Roll_L",          "ID": 4,   "Publish": 0, "dir": 1},
#     {"name": "Hip_Yaw_L",           "ID": 5,   "Publish": 0, "dir": 1},
#     {"name": "Knee_Pitch_L",        "ID": 6,   "Publish": 0, "dir": 1},
#     {"name": "Ankle_Pitch_L",       "ID": 7,   "Publish": 0, "dir": 1},
#     {"name": "Ankle_Roll_L",        "ID": 8,   "Publish": 0, "dir": 1},
#     {"name": "Hip_Pitch_R",         "ID": 9,   "Publish": 0, "dir": 1},
#     {"name": "Hip_Roll_R",          "ID": 10,  "Publish": 0, "dir": 1},
#     {"name": "Hip_Yaw_R",           "ID": 11,  "Publish": 0, "dir": 1},
#     {"name": "Knee_Pitch_R",        "ID": 12,  "Publish": 0, "dir": 1},
#     {"name": "Ankle_Pitch_R",       "ID": 13,  "Publish": 0, "dir": 1},
#     {"name": "Ankle_Roll_R",        "ID": 14,  "Publish": 0, "dir": 1},
#     {"name": "Shoulder_Pitch_L",    "ID": 15,  "Publish": 1, "dir": 1},
#     {"name": "Shoulder_Roll_L",     "ID": 16,  "Publish": 1, "dir": -1},
#     {"name": "Shoulder_Yaw_L",      "ID": 17,  "Publish": 1, "dir": -1},
#     {"name": "Elbow_Pitch_L",       "ID": 18,  "Publish": 1, "dir": -1},
#     {"name": "Wrist_Yaw_L",         "ID": 19,  "Publish": 1, "dir": -1},
#     {"name": "Wrist_Roll_L",        "ID": 20,  "Publish": 1, "dir": 1},
#     {"name": "Wrist_Pitch_L",       "ID": 21,  "Publish": 1, "dir": 1},
#     {"name": "Shoulder_Pitch_R",    "ID": 22,  "Publish": 1, "dir": -1},
#     {"name": "Shoulder_Roll_R",     "ID": 23,  "Publish": 1, "dir": 1},
#     {"name": "Shoulder_Yaw_R",      "ID": 24,  "Publish": 1, "dir": -1},
#     {"name": "Elbow_Pitch_R",       "ID": 25,  "Publish": 1, "dir": 1},
#     {"name": "Wrist_Yaw_R",         "ID": 26,  "Publish": 1, "dir": -1},
#     {"name": "Wrist_Roll_R",        "ID": 27,  "Publish": 1, "dir": 1},
#     {"name": "Wrist_Pitch_R",       "ID": 28,  "Publish": 1, "dir": 1},
#     {"name": "Neck_Yaw",            "ID": 29,  "Publish": 0, "dir": 1},
#     {"name": "Neck_Pitch",          "ID": 30,  "Publish": 0, "dir": 1},
#     {"name": "Gripper_L",           "ID": 31,  "Publish": 1, "dir": 1},
#     {"name": "Gripper_R",           "ID": 32,  "Publish": 1, "dir": 1},
# ]

robot_params = [
    {"name": "Waist_Yaw",           "ID": 0,   "Publish": 0, "dir": 1},
    {"name": "Waist_Roll",          "ID": 1,   "Publish": 0, "dir": 1},
    {"name": "Waist_Pitch",         "ID": 2,   "Publish": 0, "dir": 1},
    {"name": "Hip_Pitch_L",         "ID": 3,   "Publish": 0, "dir": 1},
    {"name": "Hip_Roll_L",          "ID": 4,   "Publish": 0, "dir": 1},
    {"name": "Hip_Yaw_L",           "ID": 5,   "Publish": 0, "dir": 1},
    {"name": "Knee_Pitch_L",        "ID": 6,   "Publish": 0, "dir": 1},
    {"name": "Ankle_Pitch_L",       "ID": 7,   "Publish": 0, "dir": 1},
    {"name": "Ankle_Roll_L",        "ID": 8,   "Publish": 0, "dir": 1},
    {"name": "Hip_Pitch_R",         "ID": 9,   "Publish": 0, "dir": 1},
    {"name": "Hip_Roll_R",          "ID": 10,  "Publish": 0, "dir": 1},
    {"name": "Hip_Yaw_R",           "ID": 11,  "Publish": 0, "dir": 1},
    {"name": "Knee_Pitch_R",        "ID": 12,  "Publish": 0, "dir": 1},
    {"name": "Ankle_Pitch_R",       "ID": 13,  "Publish": 0, "dir": 1},
    {"name": "Ankle_Roll_R",        "ID": 14,  "Publish": 0, "dir": 1},
    {"name": "Shoulder_Pitch_L",    "ID": 15,  "Publish": 1, "dir": -1},
    {"name": "Shoulder_Roll_L",     "ID": 16,  "Publish": 1, "dir": -1},
    {"name": "Shoulder_Yaw_L",      "ID": 17,  "Publish": 1, "dir": 1},
    {"name": "Elbow_Pitch_L",       "ID": 18,  "Publish": 1, "dir": -1},
    {"name": "Wrist_Yaw_L",         "ID": 19,  "Publish": 1, "dir": 1},
    {"name": "Wrist_Roll_L",        "ID": 20,  "Publish": 1, "dir": 1},
    {"name": "Wrist_Pitch_L",       "ID": 21,  "Publish": 1, "dir": 1},
    {"name": "Shoulder_Pitch_R",    "ID": 22,  "Publish": 1, "dir": 1},
    {"name": "Shoulder_Roll_R",     "ID": 23,  "Publish": 1, "dir": -1},
    {"name": "Shoulder_Yaw_R",      "ID": 24,  "Publish": 1, "dir": 1},
    {"name": "Elbow_Pitch_R",       "ID": 25,  "Publish": 1, "dir": 1},
    {"name": "Wrist_Yaw_R",         "ID": 26,  "Publish": 1, "dir": 1},
    {"name": "Wrist_Roll_R",        "ID": 27,  "Publish": 1, "dir": 1},
    {"name": "Wrist_Pitch_R",       "ID": 28,  "Publish": 1, "dir": 1},
    {"name": "Neck_Yaw",            "ID": 29,  "Publish": 0, "dir": 1},
    {"name": "Neck_Pitch",          "ID": 30,  "Publish": 0, "dir": 1},
    {"name": "Fing_th_L",           "ID": 31,  "Publish": 1, "dir": -1},
    {"name": "Fing_all_L",           "ID": 32,  "Publish": 1, "dir": 1},
    {"name": "Fing_th_R",           "ID": 33,  "Publish": 1, "dir": 1},
    {"name": "Fing_all_R",           "ID": 34,  "Publish": 1, "dir": -1},
]

# Dynamixel Address (X-Series)
ADDR_TORQUE_ENABLE      = 64
ADDR_PROFILE_VELOCITY   = 112
ADDR_GOAL_POSITION      = 116
ADDR_PRESENT_POSITION   = 132

# Byte Length
LEN_TORQUE_ENABLE       = 1
LEN_PROFILE_VELOCITY    = 4
LEN_GOAL_POSITION       = 4
LEN_PRESENT_POSITION    = 4

PROTOCOL_VERSION = 2.0
DEVICENAME = '/dev/ttyUSB0'
BAUDRATE = 3000000
# BAUDRATE = 4000000

POSITION_CENTER = 2048

# -------------------------------------------------------------------
# [유틸리티] 변환 함수
# -------------------------------------------------------------------
def dxl_to_radian(dxl_value):
    return (dxl_value - POSITION_CENTER) * (2 * 3.14159) / 4096

def radian_to_dxl(radian):
    val = int(radian * 4096 / (2 * 3.14159)) + POSITION_CENTER
    return max(0, min(4095, val))

def split_to_bytes(value, length):
    """정수를 바이트 리스트로 변환 (Little Endian)"""
    bytes_list = []
    for i in range(length):
        bytes_list.append((value >> (8 * i)) & 0xFF)
    return bytes_list

# -------------------------------------------------------------------
# [클래스] RobotController
# -------------------------------------------------------------------
class RobotController(Node):
    def __init__(self):
        super().__init__('robot_controller')
        
        # 이름으로 ID 찾기용 룩업 테이블
        self.joint_map = {j["name"]: j["ID"] for j in robot_params}
        
        # Publish할 모터 리스트
        self.publish_joints = [j for j in robot_params if j["Publish"] == 1]
        self.publish_motor_ids = [j["ID"] for j in self.publish_joints]
        
        # 위치 저장용 (초기값 0.0)
        self.current_positions = {mid: 0.0 for mid in self.publish_motor_ids}

        # ===== 추가: 이전 값 저장용 =====
        self.previous_positions = {mid: 0.0 for mid in self.publish_motor_ids}
        self.max_position_change = 0.5  # 한 번에 최대 0.5 라디안 변화만 허용
        
        # IGRIS 로봇 상태 저장
        self.igris_positions = [0.0] * 31
        self.igris_data_received = False
        self.igris_lock = threading.Lock()
        
        # 시리얼 통신 충돌 방지용 Lock
        self.dxl_lock = threading.Lock()
        
        # IGRIS 로봇 상태 저장
        self.igris_positions = [0.0] * 31
        self.igris_data_received = False
        self.igris_lock = threading.Lock()
        
        # 시리얼 통신 충돌 방지용 Lock
        self.dxl_lock = threading.Lock()
        
        # Dynamixel 초기화
        self.setup_dynamixel()
        
        # ===== [중요] Read Loop 먼저 시작 =====
        # initialize_robot에서 현재 위치를 확인하려면 읽기 스레드가 먼저 돌고 있어야 함
        self.read_thread_running = True
        self.read_thread = threading.Thread(target=self.read_loop, daemon=True)
        self.read_thread.start()
        
        # 첫 데이터를 읽을 시간 확보 (0.2초)
        time.sleep(0.2)
        
        # ===== 1. 초기화 시퀀스 실행 =====
        self.initialize_robot()
        
        # 2. Publisher
        self.publisher_ = self.create_publisher(JointState, '/igris/target_joint_states', 10)
        
        # 3. Subscriber - IGRIS 로봇 상태 구독
        self.igris_subscriber = self.create_subscription(
            JointState,
            '/igris/joint_states',
            self.igris_callback,
            10
        )
        self.get_logger().info("Subscribed to /igris/joint_states")
        
        # 4. Services
        self.srv_torque = self.create_service(SetBool, 'set_torque', self.callback_set_torque)
        self.create_service(MoveJoints, 'move_joints', self.callback_move_joints)
        
        # 5. Timer (Publish)
        self.timer = self.create_timer(0.01, self.timer_callback)
        
        self.get_logger().info('Robot Controller Started!')

    def setup_dynamixel(self):
        self.portHandler = PortHandler(DEVICENAME)
        self.packetHandler = PacketHandler(PROTOCOL_VERSION)
        
        if not self.portHandler.openPort():
            raise Exception("Port open failed")
        if not self.portHandler.setBaudRate(BAUDRATE):
            raise Exception("Baudrate setting failed")
            
        # SyncRead 설정
        self.groupSyncRead = GroupSyncRead(
            self.portHandler, self.packetHandler, 
            ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION
        )
        for mid in self.publish_motor_ids:
            self.groupSyncRead.addParam(mid)

        # SyncWrite 설정
        self.groupSyncWriteTorque = GroupSyncWrite(
            self.portHandler, self.packetHandler, 
            ADDR_TORQUE_ENABLE, LEN_TORQUE_ENABLE
        )
        self.groupSyncWritePos = GroupSyncWrite(
            self.portHandler, self.packetHandler, 
            ADDR_GOAL_POSITION, LEN_GOAL_POSITION
        )
        self.groupSyncWriteVel = GroupSyncWrite(
            self.portHandler, self.packetHandler, 
            ADDR_PROFILE_VELOCITY, LEN_PROFILE_VELOCITY
        )
        
        # 초기 설정: Return Delay Time 최적화
        self.optimize_communication()

    def optimize_communication(self):
        """통신 속도 최적화 (Return Delay Time -> 0)"""
        with self.dxl_lock:
            for mid in self.publish_motor_ids:
                self.packetHandler.write1ByteTxRx(self.portHandler, mid, 9, 0)
        self.get_logger().info("Optimized Return Delay Time.")

    # ===== 초기화 시퀀스 (수정됨) =====
    def initialize_robot(self):
        """
        1. 모든 모터 토크 ON
        2. 느린 속도로 모든 조인트를 0으로 이동
        3. 그리퍼(31, 32) 토크 OFF
        4. 그리퍼가 특정 위치(<0)라면 대기
        5. 전체 토크 OFF
        """
        self.get_logger().info("=" * 50)
        self.get_logger().info("Starting Robot Initialization...")
        self.get_logger().info("=" * 50)
        
        # Step 1: 토크 켜기
        self.get_logger().info("Step 1: Enabling torque for all motors...")
        self.set_all_torque(True)
        time.sleep(0.5)
        
        # Step 2: 모든 조인트를 0으로 천천히 이동
        self.get_logger().info("Step 2: Moving all joints to zero position (slow)...")
        
        all_ids = [j["ID"] for j in robot_params]
        zero_positions = [0.0] * len(all_ids)
        slow_velocity = 50  # 느린 속도
        
        success = self.move_joints_internal(all_ids, zero_positions, slow_velocity)
        
        # 이동할 시간 충분히 대기
        time.sleep(1.0) 

        # Step 3: 그리퍼(31, 32)만 토크 끄기
        self.get_logger().info("Step 3: Turning off torque for grippers (31, 32)...")
        self.set_torque_single(31, False)
        self.set_torque_single(32, False)

        # Step 4: 그리퍼 위치 대기 (31번 또는 32번이 0보다 작으면 대기)
        # self.current_positions는 read_loop 스레드에 의해 백그라운드에서 계속 업데이트됨
        self.get_logger().info("Step 4: Checking gripper positions...")

        gripper_L = self.current_positions.get(31, 0.0)
        gripper_R = self.current_positions.get(32, 0.0)
        

        time.sleep(5)
        # while (gripper_L < 0):
        #     # CPU 점유율 폭주 방지
        #     print(gripper_L, gripper_R)
        #     gripper_L = self.current_positions.get(31, 0.0)
        #     gripper_R = self.current_positions.get(32, 0.0)
        #     # print(self.current_positions)
        #     time.sleep(0.1)

        # Step 5: 모든 토크 해제
        self.get_logger().info("Step 5: Initialization Done. Disabling all torque...")
        self.set_all_torque(False)
        
        if success:
            self.get_logger().info("✓ Initialization complete!")
        else:
            self.get_logger().warn("⚠ Initialization completed with warnings.")
        
        self.get_logger().info("=" * 50)

    # ===== 개별 모터 토크 제어 (추가됨) =====
    def set_torque_single(self, motor_id, enable):
        """개별 모터의 토크를 제어 (패킷 직접 전송)"""
        val = 1 if enable else 0
        with self.dxl_lock:
            # GroupSyncWrite가 아닌 개별 Write 사용
            self.packetHandler.write1ByteTxRx(
                self.portHandler, motor_id, ADDR_TORQUE_ENABLE, val
            )

    # ===== 전체 모터 토크 제어 =====
    def set_all_torque(self, enable):
        """모든 모터의 토크를 켜거나 끔"""
        torque_val = 1 if enable else 0
        param_bytes = [torque_val]
        
        with self.dxl_lock:
            self.groupSyncWriteTorque.clearParam()
            for item in robot_params:
                mid = item["ID"]
                self.groupSyncWriteTorque.addParam(mid, param_bytes)
            comm_result = self.groupSyncWriteTorque.txPacket()
            
        if comm_result == COMM_SUCCESS:
            return True
        else:
            self.get_logger().error("✗ Failed to set torque")
            return False

    # ===== IGRIS 로봇 상태 콜백 =====
    def igris_callback(self, msg):
        """IGRIS 로봇의 joint_states를 받아서 저장"""
        with self.igris_lock:
            if len(msg.position) >= 31:
                self.igris_positions = list(msg.position[:31])
                if not self.igris_data_received:
                    self.igris_data_received = True
                    self.get_logger().info(f"✓ First IGRIS joint state received! ({len(msg.position)} joints)")
            else:
                # 데이터 개수가 안 맞으면 경고 (초반에 몇 번 뜰 수 있음)
                self.get_logger().warn(f"⚠ IGRIS joint mismatch: expected >=31, got {len(msg.position)}", throttle_duration_sec=2.0)

    # -------------------------------------------------------------------
    # [Read Loop] 지속적인 상태 읽기
    # -------------------------------------------------------------------
    # def read_loop(self):
    #     while self.read_thread_running:
    #         with self.dxl_lock:
    #             dxl_comm_result = self.groupSyncRead.txRxPacket()
            
    #         if dxl_comm_result == COMM_SUCCESS:
    #             new_positions = {}
    #             for mid in self.publish_motor_ids:
    #                 dxl_value = self.groupSyncRead.getData(mid, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION)
    #                 new_positions[mid] = dxl_to_radian(dxl_value)
                
    #             # Dictionary 업데이트 (Atomic in Python)
    #             self.current_positions = new_positions
            
    #         time.sleep(0.001)

    def read_loop(self):
        while self.read_thread_running:
            try:
                with self.dxl_lock:
                    dxl_comm_result = self.groupSyncRead.txRxPacket()
                
                if dxl_comm_result == COMM_SUCCESS:
                    new_positions = {}
                    
                    for mid in self.publish_motor_ids:
                        # ===== 데이터 가용성 확인 =====
                        if not self.groupSyncRead.isAvailable(mid, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION):
                            # 데이터 없으면 이전 값 유지
                            new_positions[mid] = self.previous_positions.get(mid, 0.0)
                            continue
                        
                        # ===== 값 읽기 =====
                        dxl_value = self.groupSyncRead.getData(
                            mid, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION
                        )
                        
                        # ===== 1. 범위 검증 (Dynamixel은 0~4095) =====
                        if dxl_value < 0 or dxl_value > 4095:
                            self.get_logger().warn(
                                f"Invalid DXL value for motor {mid}: {dxl_value}. Using previous value.",
                                throttle_duration_sec=1.0
                            )
                            new_positions[mid] = self.previous_positions.get(mid, 0.0)
                            continue
                        
                        # ===== 2. 라디안 변환 =====
                        rad_value = dxl_to_radian(dxl_value)
                        
                        # ===== 3. 급격한 변화 감지 (rate limiting) =====
                        prev_value = self.previous_positions.get(mid, rad_value)
                        delta = abs(rad_value - prev_value)
                        
                        # 급격한 변화는 무시 (통신 에러로 간주)
                        if delta > self.max_position_change:
                            self.get_logger().warn(
                                f"Motor {mid}: Sudden jump detected! "
                                f"prev={prev_value:.3f}, new={rad_value:.3f}, "
                                f"delta={delta:.3f}. Ignoring.",
                                throttle_duration_sec=1.0
                            )
                            new_positions[mid] = prev_value  # 이전 값 유지
                        else:
                            new_positions[mid] = rad_value
                    
                    # 업데이트
                    self.previous_positions = new_positions.copy()
                    self.current_positions = new_positions
                
                time.sleep(0.001)
                
            except Exception as e:
                self.get_logger().error(f"Read loop error: {str(e)}", throttle_duration_sec=5.0)
                time.sleep(0.01)

    # -------------------------------------------------------------------
    # [Service 1] 토크 On/Off
    # -------------------------------------------------------------------
    def callback_set_torque(self, request, response):
        success = self.set_all_torque(request.data)
        
        response.success = success
        if success:
            response.message = f"Torque set to {request.data} for all motors."
        else:
            response.message = "Failed to send SyncWrite Torque packet."
            
        return response

    # -------------------------------------------------------------------
    # [Service 2] 모터 이동
    # -------------------------------------------------------------------
    def move_joints_internal(self, ids, positions, velocity):
        """내부 함수: 실제 모터 이동 로직"""
        if len(ids) != len(positions):
            return False

        target_ids = []
        target_pos_bytes = []
        target_vel_bytes = []

        try:
            vel_int = int(velocity)
            vel_bytes = split_to_bytes(vel_int, 4)
            
            for motor_id, pos_rad in zip(ids, positions):
                pos_int = radian_to_dxl(pos_rad)
                pos_bytes = split_to_bytes(pos_int, 4)
                
                target_ids.append(motor_id)
                target_pos_bytes.append(pos_bytes)
                target_vel_bytes.append(vel_bytes)
            
            if not target_ids:
                return False

            with self.dxl_lock:
                # 속도 설정
                self.groupSyncWriteVel.clearParam()
                for i, mid in enumerate(target_ids):
                    self.groupSyncWriteVel.addParam(mid, target_vel_bytes[i])
                self.groupSyncWriteVel.txPacket()
                
                # 위치 설정
                self.groupSyncWritePos.clearParam()
                for i, mid in enumerate(target_ids):
                    self.groupSyncWritePos.addParam(mid, target_pos_bytes[i])
                comm_result = self.groupSyncWritePos.txPacket()

            return comm_result == COMM_SUCCESS
                
        except Exception as e:
            self.get_logger().error(f"Move joints error: {str(e)}")
            return False

    def callback_move_joints(self, request, response):
        """Service 콜백"""
        success = self.move_joints_internal(request.ids, request.positions, request.velocity)
        response.success = success
        response.message = "Moved" if success else "Failed"
        return response

    # -------------------------------------------------------------------
    # [Timer] Joint State 발행
    # -------------------------------------------------------------------
    # def timer_callback(self):
    #     msg = JointState()
    #     msg.header.stamp = self.get_clock().now().to_msg()
    #     msg.name = [j["name"] for j in self.publish_joints]
    #     msg.position = [self.current_positions.get(mid, 0.0) for mid in self.publish_motor_ids]
    #     self.publisher_.publish(msg)

    def timer_callback(self):
        msg = JointState()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.name = [j["name"] for j in self.publish_joints]
        
        # 발행할 때만 dir 적용! (논리적 변환)
        msg.position = [
            self.current_positions.get(j["ID"], 0.0) * j["dir"] 
            for j in self.publish_joints
        ]
        
        self.publisher_.publish(msg)

    def __del__(self):
        self.read_thread_running = False
        if hasattr(self, 'portHandler'):
            self.portHandler.closePort()

def main(args=None):
    rclpy.init(args=args)
    node = RobotController()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
