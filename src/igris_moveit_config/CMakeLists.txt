cmake_minimum_required(VERSION 3.22)
project(igris_moveit_config)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(shape_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_srvs REQUIRED)

# 충돌 회피 모션 플래닝 노드
add_executable(collision_avoidance_planner src/collision_avoidance_planner.cpp)
ament_target_dependencies(collision_avoidance_planner
  rclcpp
  moveit_ros_planning_interface
  moveit_msgs
  shape_msgs
  geometry_msgs
)
install(TARGETS collision_avoidance_planner
  DESTINATION lib/${PROJECT_NAME}
)

# Self Collision Test 노드
add_executable(self_collision_test_node src/self_collision_test_node.cpp)
ament_target_dependencies(self_collision_test_node
  rclcpp
  moveit_ros_planning_interface
  moveit_msgs
)
install(TARGETS self_collision_test_node
  DESTINATION lib/${PROJECT_NAME}
)

# Go Home 노드
add_executable(go_home_node src/go_home_node.cpp)
ament_target_dependencies(go_home_node
  rclcpp
  moveit_ros_planning_interface
  moveit_msgs
  std_srvs
)
install(TARGETS go_home_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/launch")
  install(
    DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
    PATTERN "setup_assistant.launch" EXCLUDE)
endif()

install(DIRECTORY config DESTINATION share/${PROJECT_NAME})
install(FILES .setup_assistant DESTINATION share/${PROJECT_NAME})
