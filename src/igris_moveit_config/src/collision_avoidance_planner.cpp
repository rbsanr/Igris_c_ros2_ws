#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.hpp>
#include <moveit/planning_scene_interface/planning_scene_interface.hpp>
#include <moveit_msgs/msg/collision_object.hpp>
#include <shape_msgs/msg/solid_primitive.hpp>
#include <geometry_msgs/msg/pose.hpp>

/**
 * IGRIS_C 충돌 회피 모션 플래닝 노드
 *
 * Planning Scene에 장애물(box, cylinder, sphere)을 배치한 후,
 * 왼팔/오른팔이 해당 장애물을 피해 목표 자세까지 이동하는 경로를 계획하고 실행합니다.
 */

class CollisionAvoidancePlanner : public rclcpp::Node
{
public:
  CollisionAvoidancePlanner()
  : Node("collision_avoidance_planner",
         rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true))
  {
    planning_group_ = this->get_parameter_or<std::string>("planning_group", "left_arm");
    RCLCPP_INFO(this->get_logger(), "Planning group: %s", planning_group_.c_str());
  }

  void run()
  {
    moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

    // Planning Scene에 장애물 추가
    auto collision_objects = create_obstacle_scene();
    planning_scene_interface.addCollisionObjects(collision_objects);
    RCLCPP_INFO(this->get_logger(), "%zu개의 장애물을 Planning Scene에 추가했습니다.", collision_objects.size());
    RCLCPP_INFO(this->get_logger(), "장애물이 유지됩니다. RViz에서 직접 모션 플래닝하세요. Ctrl+C로 종료하면 장애물이 제거됩니다.");
  }

private:
  std::string planning_group_;

  /**
   * 장애물 생성: Box, Cylinder, Sphere를 로봇 전방에 배치
   */
  std::vector<moveit_msgs::msg::CollisionObject> create_obstacle_scene()
  {
    std::vector<moveit_msgs::msg::CollisionObject> objects;
    std::string frame_id = "base_link";

    // --- 장애물 1: 테이블 (Box) ---
    {
      moveit_msgs::msg::CollisionObject obj;
      obj.header.frame_id = frame_id;
      obj.id = "table";

      shape_msgs::msg::SolidPrimitive primitive;
      primitive.type = primitive.BOX;
      primitive.dimensions = {0.6, 0.8, 0.02};

      geometry_msgs::msg::Pose pose;
      pose.orientation.w = 1.0;
      pose.position.x = 0.7;
      pose.position.y = 0.0;
      pose.position.z = 0.0;

      obj.primitives.push_back(primitive);
      obj.primitive_poses.push_back(pose);
      obj.operation = obj.ADD;
      objects.push_back(obj);
    }

    // --- 장애물 2: 원통형 장애물 (Cylinder) ---
    // {
    //   moveit_msgs::msg::CollisionObject obj;
    //   obj.header.frame_id = frame_id;
    //   obj.id = "cylinder_obstacle";

    //   shape_msgs::msg::SolidPrimitive primitive;
    //   primitive.type = primitive.CYLINDER;
    //   primitive.dimensions = {0.25, 0.04};  // height, radius

    //   geometry_msgs::msg::Pose pose;
    //   pose.orientation.w = 1.0;
    //   pose.position.x = 0.7;
    //   pose.position.y = 0.15;
    //   pose.position.z = 0.35;

    //   obj.primitives.push_back(primitive);
    //   obj.primitive_poses.push_back(pose);
    //   obj.operation = obj.ADD;
    //   objects.push_back(obj);
    // }

    // --- 장애물 3: 구형 장애물 (Sphere) ---
    // {
    //   moveit_msgs::msg::CollisionObject obj;
    //   obj.header.frame_id = frame_id;
    //   obj.id = "sphere_obstacle";

    //   shape_msgs::msg::SolidPrimitive primitive;
    //   primitive.type = primitive.SPHERE;
    //   primitive.dimensions = {0.06};  // radius

    //   geometry_msgs::msg::Pose pose;
    //   pose.orientation.w = 1.0;
    //   pose.position.x = 0.25;
    //   pose.position.y = -0.1;
    //   pose.position.z = 0.45;

    //   obj.primitives.push_back(primitive);
    //   obj.primitive_poses.push_back(pose);
    //   obj.operation = obj.ADD;
    //   objects.push_back(obj);
    // }

    // --- 장애물 4: 벽 (Box) ---
    {
      moveit_msgs::msg::CollisionObject obj;
      obj.header.frame_id = frame_id;
      obj.id = "wall";

      shape_msgs::msg::SolidPrimitive primitive;
      primitive.type = primitive.BOX;
      primitive.dimensions = {0.5, 0.02, 0.2};

      geometry_msgs::msg::Pose pose;
      pose.orientation.w = 1.0;
      pose.position.x = 0.7;
      pose.position.y = 0.0;
      pose.position.z = 0.2;

      obj.primitives.push_back(primitive);
      obj.primitive_poses.push_back(pose);
      obj.operation = obj.ADD;
      objects.push_back(obj);
    }

    return objects;
  }

  void plan_and_execute(
    moveit::planning_interface::MoveGroupInterface & move_group,
    const geometry_msgs::msg::Pose & target_pose)
  {
    move_group.setPoseTarget(target_pose);

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    if (move_group.plan(plan) == moveit::core::MoveItErrorCode::SUCCESS) {
      RCLCPP_INFO(this->get_logger(),
        "모션 플래닝 성공! (trajectory points: %zu) - 실행합니다.",
        plan.trajectory.joint_trajectory.points.size());
      move_group.execute(plan);
    } else {
      RCLCPP_WARN(this->get_logger(),
        "모션 플래닝 실패! 목표 pose: [%.2f, %.2f, %.2f]",
        target_pose.position.x, target_pose.position.y, target_pose.position.z);
    }
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<CollisionAvoidancePlanner>();
  node->run();

  // 노드가 계속 살아있어야 장애물이 Planning Scene에 유지됨
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
