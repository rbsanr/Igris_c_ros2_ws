#!/usr/bin/env python3
"""
Pink IK Node: VR 절대 포즈 → QP 기반 IK → joint trajectory controller
- PostureTask로 비-팔 관절 고정
- internal_q로 연속 추적 (가까운 해 보장)
- joint limit은 QP constraint로 처리
"""

import numpy as np
import pinocchio as pin
import pink
from pink import solve_ik
from pink.tasks import FrameTask, PostureTask
from pink.limits import ConfigurationLimit

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
from sensor_msgs.msg import JointState
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint
from builtin_interfaces.msg import Duration


URDF_PATH = "/home/josh/Igris_c_ros2_ws/src/urdf_igris/urdf/igris_c_v2.urdf"

RIGHT_ARM_JOINTS = [
    "22_Joint_Shoulder_Pitch_Right",
    "23_Joint_Shoulder_Roll_Right",
    "24_Joint_Shoulder_Yaw_Right",
    "25_Joint_Elbow_Pitch_Right",
    "26_Joint_Wrist_Yaw_Right",
    "27_Joint_Wrist_Roll_Right",
    "28_Joint_Wrist_Pitch_Right",
]

LEFT_ARM_JOINTS = [
    "15_Joint_Shoulder_Pitch_Left",
    "16_Joint_Shoulder_Roll_Left",
    "17_Joint_Shoulder_Yaw_Left",
    "18_Joint_Elbow_Pitch_Left",
    "19_Joint_Wrist_Yaw_Left",
    "20_Joint_Wrist_Roll_Left",
    "21_Joint_Wrist_Pitch_Left",
]


class PinkIKNode(Node):
    def __init__(self):
        super().__init__("pink_ik_node")

        # Pinocchio model
        self.model = pin.buildModelFromUrdf(URDF_PATH)
        self.data = self.model.createData()

        # Joint index mapping
        self.joint_name_to_q_idx = {}
        for i in range(1, self.model.njoints):
            name = self.model.names[i]
            self.joint_name_to_q_idx[name] = self.model.joints[i].idx_q

        self.right_arm_q_indices = [
            self.joint_name_to_q_idx[j] for j in RIGHT_ARM_JOINTS
        ]
        self.left_arm_q_indices = [
            self.joint_name_to_q_idx[j] for j in LEFT_ARM_JOINTS
        ]

        # EE Frame tasks
        self.right_hand_task = FrameTask(
            "Link_Wrist_Pitch_Right",
            position_cost=1.0,
            orientation_cost=0.2,
        )
        self.left_hand_task = FrameTask(
            "Link_Wrist_Pitch_Left",
            position_cost=1.0,
            orientation_cost=0.2,
        )

        # Posture task: 비-팔 관절을 현재 위치에 고정 (높은 cost)
        self.posture_task = PostureTask(cost=10.0)

        # Limits
        self.limits = [ConfigurationLimit(self.model)]

        self.dt = 0.01  # 100Hz

        # 상태
        self.current_q = pin.neutral(self.model)
        self.internal_q = None  # IK 내부 연속 추적용
        self.joint_state_received = False

        # Publishers
        self.right_traj_pub = self.create_publisher(
            JointTrajectory, "/right_arm_controller/joint_trajectory", 10
        )
        self.left_traj_pub = self.create_publisher(
            JointTrajectory, "/left_arm_controller/joint_trajectory", 10
        )

        # Subscribers
        self.create_subscription(
            JointState, "/joint_states", self.joint_state_cb, 10
        )
        self.create_subscription(
            PoseStamped, "/pink_ik/right_hand_pose", self.right_pose_cb, 10
        )
        self.create_subscription(
            PoseStamped, "/pink_ik/left_hand_pose", self.left_pose_cb, 10
        )

        self.get_logger().info("Pink IK Node initialized")

    def joint_state_cb(self, msg: JointState):
        for i, name in enumerate(msg.name):
            if name in self.joint_name_to_q_idx:
                self.current_q[self.joint_name_to_q_idx[name]] = msg.position[i]

        if not self.joint_state_received:
            self.joint_state_received = True
            self.internal_q = self.current_q.copy()
            # posture target = 현재 자세 (이 자세를 유지하려고 함)
            self.posture_task.set_target(self.internal_q)
            self.get_logger().info("Joint state received, IK ready")

    def pose_to_se3(self, pose_msg: PoseStamped) -> pin.SE3:
        p = pose_msg.pose.position
        o = pose_msg.pose.orientation
        quat = pin.Quaternion(o.w, o.x, o.y, o.z)
        return pin.SE3(quat.matrix(), np.array([p.x, p.y, p.z]))

    def right_pose_cb(self, msg: PoseStamped):
        if not self.joint_state_received:
            return
        target_se3 = self.pose_to_se3(msg)
        self.right_hand_task.set_target(target_se3)
        self._solve_and_publish(
            self.right_hand_task,
            RIGHT_ARM_JOINTS,
            self.right_arm_q_indices,
            self.right_traj_pub,
        )

    def left_pose_cb(self, msg: PoseStamped):
        if not self.joint_state_received:
            return
        target_se3 = self.pose_to_se3(msg)
        self.left_hand_task.set_target(target_se3)
        self._solve_and_publish(
            self.left_hand_task,
            LEFT_ARM_JOINTS,
            self.left_arm_q_indices,
            self.left_traj_pub,
        )

    def _solve_and_publish(self, ee_task, joint_names, q_indices, publisher):
        try:
            # 이전 IK 결과에서 출발 (연속 추적)
            configuration = pink.Configuration(
                self.model, self.data, self.internal_q
            )

            # EE task + posture task (비-팔 관절 고정)
            tasks = [ee_task, self.posture_task]

            velocity = solve_ik(
                configuration,
                tasks,
                self.dt,
                solver="quadprog",
                limits=self.limits,
            )

            # 적분 → 새 joint 값
            q_new = pin.integrate(self.model, self.internal_q, velocity * self.dt)
            q_new = np.clip(
                q_new,
                self.model.lowerPositionLimit,
                self.model.upperPositionLimit,
            )

            # 내부 상태 업데이트
            self.internal_q = q_new.copy()

            # 팔 joint만 추출해서 publish
            arm_positions = [float(q_new[idx]) for idx in q_indices]

            traj = JointTrajectory()
            traj.joint_names = joint_names
            point = JointTrajectoryPoint()
            point.positions = arm_positions
            point.time_from_start = Duration(sec=0, nanosec=int(self.dt * 1e9))
            traj.points = [point]
            publisher.publish(traj)

        except Exception as e:
            self.get_logger().warn(f"IK failed: {e}", throttle_duration_sec=1.0)


def main(args=None):
    rclpy.init(args=args)
    node = PinkIKNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
