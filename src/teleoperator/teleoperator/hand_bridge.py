#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_msgs.msg import String, Float32MultiArray
import threading
import json

class UnifiedHandBridge(Node):
    def __init__(self):
        super().__init__('unified_hand_bridge')
        
        # ==========================================
        # 1. Teleop 파트 초기화 (JointState -> Float32MultiArray)
        # ==========================================
        self.finger_lock = threading.Lock()
        
        # 손가락 및 그리퍼 상태 저장 (0.0~1.0)
        self.fing_th_L = 0.0    # ID: 28
        self.fing_all_L = 0.0   # ID: 29
        self.fing_th_R = 0.0    # ID: 18
        self.fing_all_R = 0.0   # ID: 19
        self.Gripper_L = 0.0
        self.Gripper_R = 0.0
        
        # Subscriber: 마스터 암/Teleop 로봇의 JointState 받기
        self.teleop_sub = self.create_subscription(
            JointState,
            '/igris/hand/joint_states',
            self.teleop_callback,
            10
        )
        
        # Publisher: IGRIS Hand 모터 목표값 전송
        self.hand_target_pub = self.create_publisher(
            Float32MultiArray,
            '/igris_c/hand/targets',
            10
        )

        # ==========================================
        # 2. Hand Status 파트 초기화 (String -> JointState)
        # ==========================================
        # 실제 로봇 핸드의 조인트 이름으로 수정이 필요합니다.
        self.joint_names = [
            'Fing_all_R', 'joint_2', 'joint_3', 'joint_4', 
            'joint_5', 'Fing_th_R', 'Fing_all_L', 'joint_8', 
            'joint_9', 'joint_10', 'joint_11', 'Fing_th_L'
        ]

        # Subscriber: IGRIS Hand 상태 JSON String 받기
        self.status_sub = self.create_subscription(
            String,
            '/igris_c/hand/status', 
            self.status_callback,
            10
        )
        
        # Publisher: 표준 JointState로 변환하여 발행
        self.joint_state_pub = self.create_publisher(
            JointState, 
            '/igris/hand/status', 
            10
        )
        
        self.get_logger().info('Unified Finger-Hand Bridge Started!')
        self.get_logger().info(' - Subscribing: /hand/joint_states, /igris_c/handtatus')
        self.get_logger().info(' - Publishing : /igris_c/hand/targets, /igris_c/joint_states')

    # ==========================================
    # Teleop 파트 콜백 및 함수
    # ==========================================
    def teleop_callback(self, msg):
        """Teleop 로봇의 손가락 값을 받아서 처리"""
        updated = False
        
        with self.finger_lock:
            for i, name in enumerate(msg.name):
                if i >= len(msg.position):
                    break
                
                if name == 'Fing_all_L':
                    self.fing_all_L = self.normalize_position(msg.position[i])
                    updated = True
                elif name == 'Fing_th_L':
                    self.fing_th_L = self.normalize_position(msg.position[i])
                    updated = True
                elif name == 'Fing_all_R':
                    self.fing_all_R = self.normalize_position(msg.position[i])
                    updated = True
                elif name == 'Fing_th_R':
                    self.fing_th_R = self.normalize_position(msg.position[i])
                    updated = True
                elif name == 'Gripper_L':
                    self.Gripper_L = self.normalize_position(msg.position[i] * -1)
                    updated = True
                elif name == 'Gripper_R':
                    self.Gripper_R = self.normalize_position(msg.position[i])
                    updated = True
        
        # 손가락 값이 업데이트되면 Hand 타겟 토픽 발행
        if updated:
            self.publish_hand_targets()
    
    def normalize_position(self, rad_value):
        """라디안 값을 0.0~1.0 범위로 정규화"""
        normalized = max(0.0, min(1.0, rad_value / 1.0))
        return normalized
    
    def publish_hand_targets(self):
        """12개 손 모터 목표값 발행"""
        with self.finger_lock:
            targets = [
                self.fing_all_R, self.fing_all_R, self.fing_all_R, 
                self.fing_all_R, self.fing_all_R, self.fing_th_R,
                self.fing_all_L, self.fing_all_L, self.fing_all_L, 
                self.fing_all_L, self.fing_all_L, self.fing_th_L,
            ]
        
        msg = Float32MultiArray()
        msg.data = [float(t) for t in targets]
        self.hand_target_pub.publish(msg)

    # ==========================================
    # Hand Status 파트 콜백
    # ==========================================
    def status_callback(self, msg):
        """JSON 형태의 String을 받아서 JointState로 변환"""
        try:
            raw_data = msg.data
            
            # 방어 코드: 문자열 앞뒤에 불필요한 포맷이 있을 경우 제거
            if raw_data.startswith("data: '"):
                raw_data = raw_data[7:-1] 

            parsed_data = json.loads(raw_data)
            
            if 'targets' in parsed_data:
                targets = parsed_data['targets']
                
                joint_state_msg = JointState()
                joint_state_msg.header.stamp = self.get_clock().now().to_msg()
                joint_state_msg.name = self.joint_names
                joint_state_msg.position = [float(val) for val in targets]
                
                self.joint_state_pub.publish(joint_state_msg)
                
        except json.JSONDecodeError:
            self.get_logger().error(f'JSON 파싱 에러 발생. 수신된 데이터: {msg.data}')
        except Exception as e:
            self.get_logger().error(f'예기치 않은 에러 발생: {e}')


def main(args=None):
    rclpy.init(args=args)
    node = UnifiedHandBridge()
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()