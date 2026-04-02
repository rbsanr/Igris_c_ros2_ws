import os
import yaml
from moveit_configs_utils import MoveItConfigsBuilder
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, TimerAction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    moveit_config = (
        MoveItConfigsBuilder("IGRIS_C", package_name="igris_moveit_config")
        .planning_pipelines(pipelines=["ompl", "pilz_industrial_motion_planner"])
        .to_moveit_configs()
    )

    pkg_share = get_package_share_directory("igris_moveit_config")

    planning_group_arg = DeclareLaunchArgument(
        "planning_group",
        default_value="left_arm",
        description="Planning group to use (left_arm or right_arm)",
    )

    # robot_state_publisher
    rsp_params_path = "/tmp/rsp_params_collision.yaml"
    with open(rsp_params_path, "w") as f:
        yaml.dump(
            {
                "robot_state_publisher": {
                    "ros__parameters": {
                        "robot_description": moveit_config.robot_description[
                            "robot_description"
                        ],
                        "publish_frequency": 50.0,
                    }
                }
            },
            f,
        )

    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        output="screen",
        parameters=[rsp_params_path],
    )

    # static_transform_publisher: world -> base_link
    static_tf = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="static_transform_publisher",
        output="screen",
        arguments=["0", "0", "0", "0", "0", "0", "world", "base_link"],
    )

    # move_group
    move_group_node = Node(
        package="moveit_ros_move_group",
        executable="move_group",
        output="screen",
        parameters=[
            moveit_config.to_dict(),
            {
                "publish_planning_scene": True,
                "publish_geometry_updates": True,
                "publish_state_updates": True,
                "publish_transforms_updates": True,
                "publish_planning_scene_hz": 10.0,
                "moveit_manage_controllers": False,
                "trajectory_execution.allowed_execution_duration_scaling": 2.0,
                "trajectory_execution.allowed_goal_duration_margin": 0.5,
                "trajectory_execution.execution_duration_monitoring": False,
            },
        ],
    )

    # ros2_control_node (mock hardware)
    ros2_control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[
            os.path.join(pkg_share, "config", "ros2_controllers.yaml"),
        ],
        remappings=[
            ("/controller_manager/robot_description", "/robot_description"),
        ],
        output="screen",
    )

    # controller spawners
    def spawner(name):
        return Node(
            package="controller_manager",
            executable="spawner",
            arguments=[name],
            output="screen",
        )

    # RViz
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="log",
        arguments=["-d", str(moveit_config.package_path / "config/moveit.rviz")],
        parameters=[
            moveit_config.planning_pipelines,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
        ],
    )

    # 충돌 회피 플래너 노드 - MoveIt config 파라미터를 직접 전달
    collision_avoidance_node = Node(
        package="igris_moveit_config",
        executable="collision_avoidance_planner",
        name="collision_avoidance_planner",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            {"planning_group": LaunchConfiguration("planning_group")},
        ],
    )

    return LaunchDescription([
        planning_group_arg,
        static_tf,
        robot_state_publisher,
        ros2_control_node,
        TimerAction(period=2.0, actions=[
            spawner("joint_state_broadcaster"),
            spawner("waist_controller"),
            spawner("left_leg_controller"),
            spawner("right_leg_controller"),
            spawner("left_arm_controller"),
            spawner("right_arm_controller"),
            spawner("head_controller"),
        ]),
        move_group_node,
        rviz_node,
        TimerAction(period=5.0, actions=[
            collision_avoidance_node,
        ]),
    ])
