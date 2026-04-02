import os
import yaml
from moveit_configs_utils import MoveItConfigsBuilder
from launch import LaunchDescription
from launch.actions import TimerAction
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
    rsp_params_path = "/tmp/rsp_params.yaml"
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
    move_group_params = {
        "publish_planning_scene": True,
        "publish_geometry_updates": True,
        "publish_state_updates": True,
        "publish_transforms_updates": True,
        "publish_planning_scene_hz": 10.0,
        # 실행 완료 후 원점 복귀 방지
        "moveit_manage_controllers": False,
        "trajectory_execution.allowed_execution_duration_scaling": 2.0,
        "trajectory_execution.allowed_goal_duration_margin": 0.5,
        "trajectory_execution.execution_duration_monitoring": False,
    }

    move_group_node = Node(
        package="moveit_ros_move_group",
        executable="move_group",
        output="screen",
        parameters=[
            moveit_config.to_dict(),
            move_group_params,
        ],
    )

    # 4. left servo node
    left_servo_node = Node(
        package="moveit_servo",
        executable="servo_node",
        name="left_servo",
        namespace="left_servo",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            os.path.join(pkg_share, "config", "left_servo_params.yaml"),
        ],
    )

    # 5. right servo node
    right_servo_node = Node(
        package="moveit_servo",
        executable="servo_node",
        name="right_servo",
        namespace="right_servo",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            os.path.join(pkg_share, "config", "right_servo_params.yaml"),
        ],
    )

    # 6. head servo node
    head_servo_node = Node(
        package="moveit_servo",
        executable="servo_node",
        name="head_servo",
        namespace="head_servo",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            os.path.join(pkg_share, "config", "head_servo_params.yaml"),
        ],
    )

    # 7. waist servo node
    waist_servo_node = Node(
        package="moveit_servo",
        executable="servo_node",
        name="waist_servo",
        namespace="waist_servo",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            os.path.join(pkg_share, "config", "waist_servo_params.yaml"),
        ],
    )

    # 8. left leg servo node
    left_leg_servo_node = Node(
        package="moveit_servo",
        executable="servo_node",
        name="left_leg_servo",
        namespace="left_leg_servo",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            os.path.join(pkg_share, "config", "left_leg_servo_params.yaml"),
        ],
    )

    # 9. right leg servo node
    right_leg_servo_node = Node(
        package="moveit_servo",
        executable="servo_node",
        name="right_leg_servo",
        namespace="right_leg_servo",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            os.path.join(pkg_share, "config", "right_leg_servo_params.yaml"),
        ],
    )

    # 10. ros2_control_node (mock hardware)
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

    # 7. controller spawners
    def spawner(name):
        return Node(
            package="controller_manager",
            executable="spawner",
            arguments=[name],
            output="screen",
        )

    joint_state_broadcaster_spawner = spawner("joint_state_broadcaster")
    waist_spawner         = spawner("waist_controller")
    left_leg_spawner      = spawner("left_leg_controller")
    right_leg_spawner     = spawner("right_leg_controller")
    left_arm_spawner      = spawner("left_arm_controller")
    right_arm_spawner     = spawner("right_arm_controller")
    head_spawner          = spawner("head_controller")

    # 8. RViz
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

    return LaunchDescription([
        static_tf,
        robot_state_publisher,
        ros2_control_node,
        TimerAction(period=2.0, actions=[
            joint_state_broadcaster_spawner,
            waist_spawner,
            left_leg_spawner,
            right_leg_spawner,
            left_arm_spawner,
            right_arm_spawner,
            head_spawner,
        ]),
        move_group_node,
        rviz_node,
        TimerAction(period=5.0, actions=[
            left_servo_node,
            right_servo_node,
            # head_servo_node,
            # waist_servo_node,
            left_leg_servo_node,
            right_leg_servo_node,
        ]),
    ])
