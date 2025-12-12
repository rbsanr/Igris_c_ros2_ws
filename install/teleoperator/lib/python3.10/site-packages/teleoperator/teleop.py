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
robot_params = [
    {"name": "Joint_Waist_Yaw",             "ID": 0,        "Publish": 0},
    {"name": "Joint_Waist_Roll",            "ID": 1,        "Publish": 0},
    {"name": "Joint_Waist_Pitch",           "ID": 2,        "Publish": 0},
    {"name": "Joint_Hip_Pitch_Left",        "ID": 3,        "Publish": 0},
    {"name": "Joint_Hip_Roll_Left",         "ID": 4,        "Publish": 0},
    {"name": "Joint_Hip_Yaw_Left",          "ID": 5,        "Publish": 0},
    {"name": "Joint_Knee_Pitch_Left",       "ID": 6,        "Publish": 0},
    {"name": "Joint_Ankle_Pitch_Left",      "ID": 7,        "Publish": 0},
    {"name": "Joint_Ankle_Roll_Left",       "ID": 8,        "Publish": 0},
    {"name": "Joint_Hip_Pitch_Right",       "ID": 9,        "Publish": 0},
    {"name": "Joint_Hip_Roll_Right",        "ID": 10,       "Publish": 0},
    {"name": "Joint_Hip_Yaw_Right",         "ID": 11,       "Publish": 0},
    {"name": "Joint_Knee_Pitch_Right",      "ID": 12,       "Publish": 0},
    {"name": "Joint_Ankle_Pitch_Right",     "ID": 13,       "Publish": 0},
    {"name": "Joint_Ankle_Roll_Right",      "ID": 14,       "Publish": 0},
    {"name": "Joint_Shoulder_Pitch_Left",   "ID": 15,       "Publish": 1},
    {"name": "Joint_Shoulder_Roll_Left",    "ID": 16,       "Publish": 1},
    {"name": "Joint_Shoulder_Yaw_Left",     "ID": 17,       "Publish": 1},
    {"name": "Joint_Elbow_Pitch_Left",      "ID": 18,       "Publish": 1},
    {"name": "Joint_Wrist_Yaw_Left",        "ID": 19,       "Publish": 1},
    {"name": "Joint_Wrist_Roll_Left",       "ID": 20,       "Publish": 1},
    {"name": "Joint_Wrist_Pitch_Left",      "ID": 21,       "Publish": 1},
    {"name": "Joint_Shoulder_Pitch_Right",  "ID": 22,       "Publish": 1},
    {"name": "Joint_Shoulder_Roll_Right",   "ID": 23,       "Publish": 1},
    {"name": "Joint_Shoulder_Yaw_Right",    "ID": 24,       "Publish": 1},
    {"name": "Joint_Elbow_Pitch_Right",     "ID": 25,       "Publish": 1},
    {"name": "Joint_Wrist_Yaw_Right",       "ID": 26,       "Publish": 1},
    {"name": "Joint_Wrist_Roll_Right",      "ID": 27,       "Publish": 1},
    {"name": "Joint_Wrist_Pitch_Right",     "ID": 28,       "Publish": 1},
    {"name": "Joint_Neck_Yaw",              "ID": 29,       "Publish": 0},
    {"name": "Joint_Neck_Pitch",            "ID": 30,       "Publish": 0},
    {"name": "Joint_Gripper_Left",          "ID": 31,       "Publish": 1},
    {"name": "Joint_Gripper_Right",         "ID": 32,       "Publish": 1},
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
BAUDRATE = 4000000
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
        
        # 위치 저장용
        self.current_positions = {mid: 0.0 for mid in self.publish_motor_ids}
        
        # *** 중요: 시리얼 통신 충돌 방지용 Lock ***
        # Read 쓰레드와 Service(Write)가 동시에 포트에 접근하면 안됨
        self.dxl_lock = threading.Lock()
        
        # Dynamixel 초기화
        self.setup_dynamixel()
        
        # 1. Publisher
        self.publisher_ = self.create_publisher(JointState, 'joint_states', 10)
        
        # 2. Services
        # (A) 토크 제어 서비스 (/set_torque) - std_srvs/SetBool 사용
        self.srv_torque = self.create_service(SetBool, 'set_torque', self.callback_set_torque)
        
        # (B) 이동 서비스 (여기서는 예시로 커스텀 서비스 로직 구현)
        self.create_service(MoveJoints, 'move_joints', self.callback_move_joints)
        self.get_logger().info("Service Ready: /set_torque")
        
        # 3. Read Loop 시작
        self.read_thread_running = True
        self.read_thread = threading.Thread(target=self.read_loop, daemon=True)
        self.read_thread.start()
        
        # 4. Timer (Publish)
        self.timer = self.create_timer(0.01, self.timer_callback)
        
        self.get_logger().info('Robot Controller Started!')

    def setup_dynamixel(self):
        self.portHandler = PortHandler(DEVICENAME)
        self.packetHandler = PacketHandler(PROTOCOL_VERSION)
        
        if not self.portHandler.openPort():
            raise Exception("Port open failed")
        if not self.portHandler.setBaudRate(BAUDRATE):
            raise Exception("Baudrate setting failed")
            
        # -----------------------------------------------------
        # [SyncRead 설정]
        # -----------------------------------------------------
        self.groupSyncRead = GroupSyncRead(
            self.portHandler, self.packetHandler, 
            ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION
        )
        for mid in self.publish_motor_ids:
            self.groupSyncRead.addParam(mid)

        # -----------------------------------------------------
        # [SyncWrite 설정] - 토크, 위치, 속도 제어용
        # -----------------------------------------------------
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
        
        # 초기 설정: Latency Timer, Return Delay Time 최적화
        self.optimize_communication()

    def optimize_communication(self):
        """통신 속도 최적화 (Return Delay Time -> 0)"""
        with self.dxl_lock:
            for mid in self.publish_motor_ids:
                self.packetHandler.write1ByteTxRx(self.portHandler, mid, 9, 0)
        self.get_logger().info("Optimized Return Delay Time.")

    # -------------------------------------------------------------------
    # [Read Loop] 지속적인 상태 읽기
    # -------------------------------------------------------------------
    def read_loop(self):
        while self.read_thread_running:
            start = time.time()
            
            # *** LOCK 사용 ***
            with self.dxl_lock:
                dxl_comm_result = self.groupSyncRead.txRxPacket()
            
            if dxl_comm_result == COMM_SUCCESS:
                new_positions = {}
                for mid in self.publish_motor_ids:
                    dxl_value = self.groupSyncRead.getData(mid, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION)
                    new_positions[mid] = dxl_to_radian(dxl_value)
                
                # Lock 없이 Dictionary 업데이트 (Atomic에 가까움)
                self.current_positions = new_positions
            
            # CPU 점유율 조절 (Latency가 1ms라면 sleep 없이도 가능)
            time.sleep(0.001)

    # -------------------------------------------------------------------
    # [Service 1] 토크 On/Off (SyncWrite)
    # -------------------------------------------------------------------
    def callback_set_torque(self, request, response):
        """
        req.data = True (Torque On), False (Torque Off)
        모든 모터에 대해 일괄 적용
        """
        torque_val = 1 if request.data else 0
        param_bytes = [torque_val] # 1 byte
        
        with self.dxl_lock: # 통신 중 다른 작업 차단
            self.groupSyncWriteTorque.clearParam()
            
            for item in robot_params:
                mid = item["ID"]
                self.groupSyncWriteTorque.addParam(mid, param_bytes)
            
            comm_result = self.groupSyncWriteTorque.txPacket()
            
        if comm_result == COMM_SUCCESS:
            response.success = True
            response.message = f"Torque set to {request.data} for all motors."
        else:
            response.success = False
            response.message = "Failed to send SyncWrite Torque packet."
            
        return response

    # -------------------------------------------------------------------
    # [Service 2] 모터 이동 (속도 + 위치)
    # -------------------------------------------------------------------
    def callback_move_joints(self, request, response):
        """
        [ID 기반 제어]
        request.ids: 움직일 모터 ID 리스트 (예: [15, 16])
        request.positions: 목표 위치 리스트 (예: [0.0, 1.57])
        request.velocity: 속도
        """
        # 1. request에서 데이터 꺼내기
        ids = request.ids
        positions = request.positions
        velocity = request.velocity
        
        # -----------------------------------------------------
        # [검증] ID 개수와 위치 개수가 다르면 거부
        # -----------------------------------------------------
        if len(ids) != len(positions):
            response.success = False
            response.message = "Error: The number of IDs and Positions must be same."
            self.get_logger().error(response.message)
            return response

        target_ids = []
        target_pos_bytes = []
        target_vel_bytes = []

        try:
            # 공통 속도 변환
            vel_int = int(velocity)
            vel_bytes = split_to_bytes(vel_int, 4)
            
            valid_cnt = 0
            
            # 2. 루프: ID와 Position을 바로 매칭
            for motor_id, pos_rad in zip(ids, positions):
                
                # 안전장치: 우리가 관리하는 모터인지 확인 (선택사항)
                # if motor_id not in self.publish_motor_ids:
                #     self.get_logger().warn(f"Warning: ID {motor_id} is not in robot_params.")
                #     continue

                # Radian -> DXL 값 변환
                pos_int = radian_to_dxl(pos_rad)
                pos_bytes = split_to_bytes(pos_int, 4)
                
                # 전송 리스트에 추가
                target_ids.append(motor_id)
                target_pos_bytes.append(pos_bytes)
                target_vel_bytes.append(vel_bytes)
                valid_cnt += 1
            
            if valid_cnt == 0:
                response.success = False
                response.message = "No valid IDs provided."
                return response

            # 3. 통신 (SyncWrite)
            with self.dxl_lock:
                # (1) 속도 설정
                self.groupSyncWriteVel.clearParam()
                for i, mid in enumerate(target_ids):
                    self.groupSyncWriteVel.addParam(mid, target_vel_bytes[i])
                self.groupSyncWriteVel.txPacket()
                
                # (2) 위치 설정
                self.groupSyncWritePos.clearParam()
                for i, mid in enumerate(target_ids):
                    self.groupSyncWritePos.addParam(mid, target_pos_bytes[i])
                comm_result = self.groupSyncWritePos.txPacket()

            # 4. 결과 리턴
            if comm_result == COMM_SUCCESS:
                response.success = True
                response.message = f"Success: Moved {valid_cnt} motors."
            else:
                response.success = False
                response.message = "Failed: SyncWrite Packet Error."
                
        except Exception as e:
            self.get_logger().error(f"Callback Error: {str(e)}")
            response.success = False
            response.message = str(e)

        return response

    def timer_callback(self):
        msg = JointState()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.name = [j["name"] for j in self.publish_joints]
        msg.position = [self.current_positions[mid] for mid in self.publish_motor_ids]
        self.publisher_.publish(msg)
        
    def __del__(self):
        self.read_thread_running = False
        # 포트 닫기 등 정리...

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