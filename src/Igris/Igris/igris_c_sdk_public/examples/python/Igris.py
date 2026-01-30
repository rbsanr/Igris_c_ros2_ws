import sys
import time
import igris_c_sdk as igc_sdk

print(igc_sdk.__dir__())

domain_id = 0

channel_instance = igc_sdk.ChannelFactory.Instance()
channel_instance.Init(domain_id)

client = igc_sdk.IgrisC_Client()
client.Init()
client.SetTimeout(20000)

lowstate_subscriber = igc_sdk.LowStateSubscriber("rt/lowstate")
lowstate_cnt = 0

robot_params = [
    {"name": "Joint_Waist_Yaw",             "ID": 0,        "ready_pos": 0.0,    "kp": 50.0,    "kd": 0.80},
    {"name": "Joint_Waist_Roll",            "ID": 1,        "ready_pos": -0.34,  "kp": 25.0,    "kd": 0.80},
    {"name": "Joint_Waist_Pitch",           "ID": 2,        "ready_pos": -0.34,  "kp": 25.0,    "kd": 0.80},
    {"name": "Joint_Hip_Pitch_Left",        "ID": 3,        "ready_pos": 0.0,    "kp": 500.0,   "kd": 3.0},
    {"name": "Joint_Hip_Roll_Left",         "ID": 4,        "ready_pos": 0.0,    "kp": 200.0,   "kd": 0.50},
    {"name": "Joint_Hip_Yaw_Left",          "ID": 5,        "ready_pos": 0.0,    "kp": 50.0,    "kd": 0.50},
    {"name": "Joint_Knee_Pitch_Left",       "ID": 6,        "ready_pos": 0.0,    "kp": 500.0,   "kd": 3.0},
    {"name": "Joint_Ankle_Pitch_Left",      "ID": 7,        "ready_pos": 0.0,    "kp": 300.0,   "kd": 1.50},
    {"name": "Joint_Ankle_Roll_Left",       "ID": 8,        "ready_pos": 0.0,    "kp": 300.0,   "kd": 1.50},
    {"name": "Joint_Hip_Pitch_Right",       "ID": 9,        "ready_pos": 0.0,    "kp": 500.0,   "kd": 3.00},
    {"name": "Joint_Hip_Roll_Right",        "ID": 10,       "ready_pos": 0.0,    "kp": 200.0,   "kd": 0.50},
    {"name": "Joint_Hip_Yaw_Right",         "ID": 11,       "ready_pos": 0.0,    "kp": 50.0,    "kd": 0.50},
    {"name": "Joint_Knee_Pitch_Right",      "ID": 12,       "ready_pos": 0.0,    "kp": 500.0,   "kd": 3.00},
    {"name": "Joint_Ankle_Pitch_Right",     "ID": 13,       "ready_pos": 0.0,    "kp": 300.0,   "kd": 1.50},
    {"name": "Joint_Ankle_Roll_Right",      "ID": 14,       "ready_pos": 0.0,    "kp": 300.0,   "kd": 1.50},
    {"name": "Joint_Shoulder_Pitch_Left",   "ID": 15,       "ready_pos": 0.0,    "kp": 50.0,    "kd": 0.50},
    {"name": "Joint_Shoulder_Roll_Left",    "ID": 16,       "ready_pos": 0.34,   "kp": 50.0,    "kd": 0.50},
    {"name": "Joint_Shoulder_Yaw_Left",     "ID": 17,       "ready_pos": 0.0,    "kp": 30.0,    "kd": 0.15},
    {"name": "Joint_Elbow_Pitch_Left",      "ID": 18,       "ready_pos": 0.0,    "kp": 30.0,    "kd": 0.15},
    {"name": "Joint_Wrist_Yaw_Left",        "ID": 19,       "ready_pos": 0.0,    "kp": 5.0,     "kd": 0.10},
    {"name": "Joint_Wrist_Roll_Left",       "ID": 20,       "ready_pos": 0.0,    "kp": 5.0,     "kd": 0.10},
    {"name": "Joint_Wrist_Pitch_Left",      "ID": 21,       "ready_pos": 0.0,    "kp": 5.0,     "kd": 0.10},
    {"name": "Joint_Shoulder_Pitch_Right",  "ID": 22,       "ready_pos": 0.0,    "kp": 50.0,    "kd": 0.50},
    {"name": "Joint_Shoulder_Roll_Right",   "ID": 23,       "ready_pos": -0.34,  "kp": 50.0,    "kd": 0.50},
    {"name": "Joint_Shoulder_Yaw_Right",    "ID": 24,       "ready_pos": 0.0,    "kp": 30.0,    "kd": 0.15},
    {"name": "Joint_Elbow_Pitch_Right",     "ID": 25,       "ready_pos": 0.0,    "kp": 30.0,    "kd": 0.15},
    {"name": "Joint_Wrist_Yaw_Right",       "ID": 26,       "ready_pos": 0.0,    "kp": 5.0,     "kd": 0.10},
    {"name": "Joint_Wrist_Roll_Right",      "ID": 27,       "ready_pos": 0.0,    "kp": 5.0,     "kd": 0.10},
    {"name": "Joint_Wrist_Pitch_Right",     "ID": 28,       "ready_pos": 0.0,    "kp": 5.0,     "kd": 0.10},
    {"name": "Joint_Neck_Yaw",              "ID": 29,       "ready_pos": 0.0,    "kp": 2.0,     "kd": 0.05},
    {"name": "Joint_Neck_Pitch",            "ID": 30,       "ready_pos": 0.0,    "kp": 5.0,     "kd": 0.10},
    {"name": "Joint_Gripper_Left",          "ID": 31,       "ready_pos": 0.0,    "kp": 0.0,     "kd": 0.0},
    {"name": "Joint_Gripper_Right",         "ID": 32,       "ready_pos": 0.0,    "kp": 0.0,     "kd": 0.0},
]

curr_pos = [0.0]*31


# def lowstate_callback(msg):
#     imu_state = msg.imu_state()
#     quaternion = imu_state.quaternion()
#     gyroscope = imu_state.gyroscope()
#     accelerometer = imu_state.accelerometer()
#     rpy = imu_state.rpy()

#     motor_states = msg.motor_state()
#     motor_1 = motor_states[0]
#     motor_31 = motor_states[30]

#     q = motor_1.q()
#     dq = motor_1.dq()
#     tau_est = motor_1.tau_est()
#     temp = motor_1.temperature()
#     status = motor_1.status_bits()

#     joint_states = msg.joint_state()
#     joint_1 = joint_states[0]
#     joint_31 = joint_states[30]
#     q = joint_1.q()
#     dq = joint_1.dq()
#     tau_est = joint_1.tau_est()
#     status_bits = joint_1.status_bits()

#     print(f"motor 1 pos{q}")

motor_states = None

def lowstate_callback(msg):
    # 1. IMU 데이터 (기존 코드 유지)
    imu_state = msg.imu_state()
    quaternion = imu_state.quaternion()
    gyroscope = imu_state.gyroscope()
    accelerometer = imu_state.accelerometer()
    rpy = imu_state.rpy()

    # 2. 모터 상태 리스트 가져오기
    motor_states = msg.motor_state()
    
    # (선택사항) 조인트 상태가 필요하다면 아래 주석 해제
    # joint_states = msg.joint_state()

    # print("=" * 50)
    # print(f"IMU RPY: {rpy}") # IMU 상태도 같이 확인하면 좋습니다.
    
    # 3. 0번부터 30번까지 반복문으로 상태 출력
    # range(31)은 0부터 30까지 반복합니다.


    # for i in range(31):
    #     try:
    #         motor = motor_states[i]
            
    #         q = motor.q()
    #         dq = motor.dq()
    #         tau_est = motor.tau_est()
    #         temp = motor.temperature()
    #         # status = motor.status_bits() # 필요시 주석 해제

    #         # print(f"ID: {i:02d} | Pos: {q:.3f} | Vel: {dq:.3f} | Tau: {tau_est:.3f} | Temp: {temp}")
    #         curr_pos[i] = q
    #     except IndexError:
    #         print(f"Error: Motor ID {i} is out of range.")

    # print("=" * 50)


lowcmd_publisher = igc_sdk.LowCmdPublisher("rt/lowcmd")
lowcmd_publisher.init()


def call_init_bms(
    client: igc_sdk.IgrisC_Client, bms_init_type: igc_sdk.BmsInitType, type_name: str
):
    print(f"Calling Init with {type_name}...")
    res = client.InitBms(bms_init_type, 20000)

    result = "SUCCESS" if res.success() else "FAILED" + " - " + res.message()
    print(f"Init {type_name} result: {result}")


def call_set_torque(client: igc_sdk.IgrisC_Client, torque_type: igc_sdk.TorqueType, type_name: str):
    print(f"Calling Set Torque with {type_name}...")
    res = client.SetTorque(torque_type, 20000)

    result = "SUCCESS" if res.success() else "FAILED" + " - " + res.message()
    print(f"Set Torque {type_name} result: {result}")


def call_set_control_mode(
    client: igc_sdk.IgrisC_Client, control_mode: igc_sdk.ControlMode, mode_name: str
):
    print(f"Calling Set Control Mode with {mode_name}...")
    res = client.SetControlMode(control_mode, 5000)

    result = "SUCCESS" if res.success() else "FAILED" + " - " + res.message()
    print(f"Set Control Mode {mode_name} result: {result}")

def BMS_INIT():
    call_init_bms(client, igc_sdk.BmsInitType.BMS_INIT, "BMS_INIT")

def MOTOR_INIT():
    call_init_bms(client, igc_sdk.BmsInitType.MOTOR_INIT, "MOTOR_INIT")

def BMS_AND_MOTOR_INIT():
    call_init_bms(client, igc_sdk.BmsInitType.BMS_AND_MOTOR_INIT, "BMS_AND_MOTOR_INIT")

def BMS_OFF():
    call_init_bms(client, igc_sdk.BmsInitType.BMS_INIT_NONE, "BMS_OFF")

def TORQUE_ON():
    call_set_torque(client, igc_sdk.TorqueType.TORQUE_ON, "TORQUE_ON")

def TORQUE_OFF():
    call_set_torque(client, igc_sdk.TorqueType.TORQUE_OFF, "TORQUE_OFF")

def CONTROL_MODE_LOW_LEVEL():
    call_set_control_mode(
        client, igc_sdk.ControlMode.CONTROL_MODE_LOW_LEVEL, "CONTROL_MODE_LOW_LEVEL"
    )

def CONTROL_MODE_HIGH_LEVEL():
    call_set_control_mode(
        client, igc_sdk.ControlMode.CONTROL_MODE_HIGH_LEVEL, "CONTROL_MODE_HIGH_LEVEL"
    )

def set_motor_param(tau = [0.0]*31):
    low_cmd_msg = igc_sdk.LowCmd()

    # lowstate_subscriber.init(lowstate_callback)
    # time.sleep(0.1)
    # del lowstate_subscriber

    for i in range(0, 31):
        motor_cmd = low_cmd_msg.motors()[i]

        motor_cmd.q(motor_states[i].q)
        motor_cmd.dq(0.0)

        motor_cmd.tau(tau[i])
        motor_cmd.kp(robot_params[i]["kp"])
        motor_cmd.kd(robot_params[i]["kd"])
        motor_cmd.id(i)
    low_cmd_msg.kinematic_mode(igc_sdk.KinematicMode.MS)
    lowcmd_publisher.write(low_cmd_msg)

def set_motor_pose(q = [0.0]*31, dq = [0.0]*31):
    low_cmd_msg = igc_sdk.LowCmd()
    print(q)
    for i in range(0, 31):
        motor_cmd = low_cmd_msg.motors()[i]
        motor_cmd.q(q[i])
        motor_cmd.dq(dq[i])
        motor_cmd.id(i)
    low_cmd_msg.kinematic_mode(igc_sdk.KinematicMode.MS)
    lowcmd_publisher.write(low_cmd_msg)

def state_subscriber_ON():
    lowstate_subscriber.init(lowstate_callback)

def state_subscriber_OFF():
    del lowstate_subscriber


BMS_AND_MOTOR_INIT()
# print("Succes")
# time.sleep(1)
# lowstate_subscriber.init(lowstate_callback)
# time.sleep(1)
# del lowstate_subscriber
# TORQUE_ON()
# print("Succes")
# time.sleep(1)
# CONTROL_MODE_LOW_LEVEL()
# print("Succes")
# time.sleep(1)
# set_motor_pose(curr_pos)
# print("Succes")
# time.sleep(1)
# set_motor_param()

# BMS_OFF()