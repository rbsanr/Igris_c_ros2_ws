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

    # robot_description을 /tmp/rsp_params.yaml 에 기록 후 rsp에 전달
    rsp_params_path = "/tmp/rsp_params_self_collision.yaml"
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

    # 1. robot_state_publisher
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        output="screen",
        parameters=[rsp_params_path],
    )

    # 2. static_transform_publisher: world → base_link
    static_tf = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="static_transform_publisher",
        output="screen",
        arguments=["0", "0", "0", "0", "0", "0", "world", "base_link"],
    )

    # 3. move_group
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

    # 4. ros2_control_node (mock hardware)
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

    # 5. controller spawners
    def spawner(name):
        return Node(
            package="controller_manager",
            executable="spawner",
            arguments=[name],
            output="screen",
        )

    # 6. RViz
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

    repeat_arg = DeclareLaunchArgument(
        "repeat", default_value="3", description="Number of repeat cycles",
    )

    # 7. self collision test 노드 (MoveGroupInterface로 플래닝 + 충돌 체크)
    self_collision_test_node = Node(
        package="igris_moveit_config",
        executable="self_collision_test_node",
        name="self_collision_test_node",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            {"repeat": LaunchConfiguration("repeat")},
        ],
    )

    return LaunchDescription([
        repeat_arg,
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
        TimerAction(period=7.0, actions=[
            self_collision_test_node,
        ]),
    ])
