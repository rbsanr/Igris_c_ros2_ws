from moveit_configs_utils import MoveItConfigsBuilder
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    moveit_config = (
        MoveItConfigsBuilder("IGRIS_C", package_name="igris_moveit_config")
        .to_moveit_configs()
    )

    pkg_share = get_package_share_directory("igris_moveit_config")

    left_servo_node = Node(
        package="moveit_servo",
        executable="servo_node",
        name="left_servo",
        namespace="left_servo",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            os.path.join(pkg_share, "config", "left_servo_params.yaml"),
        ],
        output="screen",
    )

    right_servo_node = Node(
        package="moveit_servo",
        executable="servo_node",
        name="right_servo",
        namespace="right_servo",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            os.path.join(pkg_share, "config", "right_servo_params.yaml"),
        ],
        output="screen",
    )

    return LaunchDescription([
        left_servo_node,
        right_servo_node,
    ])
