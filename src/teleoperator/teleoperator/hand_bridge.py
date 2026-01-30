#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_msgs.msg import Float32MultiArray
import threading

class FingerToHandBridge(Node):
    def __init__(self):
        super().__init__('finger_to_hand_bridge')
        
        # 손가락 상태 저장 (0.0~1.0)
        self.finger_lock = threading.Lock()
        
        # 기존 변수들
        self.fing_all_L = 0.0   # ID: 31
        self.fing_th_L = 0.0    # ID: 32
        self.fing_all_R = 0.0   # ID: 33
        self.fing_th_R = 0.0    # ID: 34
        
        # ### 추가됨: 그리퍼 변수 초기화 ###
        # 토픽이 들어오지 않아도 변수가 존재하도록 미리 0.0으로 선언합니다.
        self.Gripper_L = 0.0
        self.Gripper_R = 0.0
        
        # 손가락 ID 매핑
        self.finger_ids = {
            'Fing_all_L': 31,
            'Fing_th_L': 32,
            'Fing_all_R': 33,
            'Fing_th_R': 34
        }
        
        # Subscriber: Teleop 로봇의 JointState 받기
        self.teleop_sub = self.create_subscription(
            JointState,
            '/igris/target_joint_states',
            self.teleop_callback,
            10
        )
        
        # Publisher: IGRIS Hand로 전송
        self.hand_pub = self.create_publisher(
            Float32MultiArray,
            '/igris_c/hand/targets',
            10
        )
        
        self.get_logger().info('Finger to Hand Bridge Started!')
        self.get_logger().info('Listening: /igris/target_joint_states')
        self.get_logger().info('Publishing: /igris_c/hand/targets')
    
    def teleop_callback(self, msg):
        """Teleop 로봇의 손가락 값을 받아서 처리"""
        
        # JointState에서 손가락 값 추출
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
                    self.Gripper_L = self.normalize_position(msg.position[i]*-1)
                    updated = True
                
                elif name == 'Gripper_R':
                    self.Gripper_R = self.normalize_position(msg.position[i])
                    updated = True
        
        # 손가락 값이 업데이트되면 Hand 토픽 발행
        if updated:
            self.publish_hand_targets()
    
    def normalize_position(self, rad_value):
        """라디안 값을 0.0~1.0 범위로 정규화"""
        # 일반적으로 손가락은 0(펼침) ~ 1.5 rad(완전히 쥠) 정도
        # 여기서는 0~1 라디안을 0.0~1.0으로 매핑
        normalized = max(0.0, min(1.0, rad_value / 1.0))
        return normalized
    
    def publish_hand_targets(self):
        """12개 손 모터 목표값 발행"""
        
        with self.finger_lock:
            # 12개 모터 매핑:
            # Gripper 변수가 초기화되어 있으므로, 데이터가 안 들어와도 0.0으로 발행됨
            targets = [
                self.Gripper_R,
                self.Gripper_R,
                self.Gripper_R,
                self.Gripper_R,
                self.Gripper_R,
                self.Gripper_R*1.5,

                self.Gripper_L,
                self.Gripper_L,
                self.Gripper_L,
                self.Gripper_L,
                self.Gripper_L,
                self.Gripper_L*1.5,
            ]
        
        # Float32MultiArray 메시지 생성
        msg = Float32MultiArray()
        msg.data = [float(t) for t in targets]
        
        self.hand_pub.publish(msg)
        
        # 디버그 로그 (5초마다) - 필요시 주석 해제하거나 수정
        # self.get_logger().debug(
        #     f'Hand targets: L_grp={self.Gripper_L:.2f}, R_grp={self.Gripper_R:.2f}'
        # )

def main(args=None):
    rclpy.init(args=args)
    node = FingerToHandBridge()
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
