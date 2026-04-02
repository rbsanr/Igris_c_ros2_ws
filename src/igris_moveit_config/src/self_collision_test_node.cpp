#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.hpp>

class SelfCollisionTestNode : public rclcpp::Node
{
public:
  SelfCollisionTestNode()
  : Node("self_collision_test_node",
         rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true))
  {
  }

  void run()
  {
    // both_arms 그룹으로 양팔 동시 플래닝
    moveit::planning_interface::MoveGroupInterface both_arms(
        shared_from_this(), "both_arms");

    both_arms.setPlanningTime(30.0);
    both_arms.setNumPlanningAttempts(5);

    // 목표 자세 1: waist(3) + left_arm(7) + right_arm(7) = 17 DOF
    std::map<std::string, double> target1 = {
        {"0_Joint_Waist_Yaw",              0.0},
        {"1_Joint_Waist_Roll",             0.0},
        {"2_Joint_Waist_Pitch",            0.0},
        {"15_Joint_Shoulder_Pitch_Left",  -0.7236},
        {"16_Joint_Shoulder_Roll_Left",    0.688},
        {"17_Joint_Shoulder_Yaw_Left",    -0.965},
        {"18_Joint_Elbow_Pitch_Left",     -1.870},
        {"19_Joint_Wrist_Yaw_Left",       -0.457},
        {"20_Joint_Wrist_Roll_Left",       0.0},
        {"21_Joint_Wrist_Pitch_Left",      0.0},
        {"22_Joint_Shoulder_Pitch_Right", -0.202},
        {"23_Joint_Shoulder_Roll_Right",  -0.633},
        {"24_Joint_Shoulder_Yaw_Right",    0.800},
        {"25_Joint_Elbow_Pitch_Right",    -1.755},
        {"26_Joint_Wrist_Yaw_Right",       0.948},
        {"27_Joint_Wrist_Roll_Right",      0.0},
        {"28_Joint_Wrist_Pitch_Right",     0.0},
    };

    // 목표 자세 2
    std::map<std::string, double> target2 = {
        {"0_Joint_Waist_Yaw",              0.0},
        {"1_Joint_Waist_Roll",             0.0},
        {"2_Joint_Waist_Pitch",            0.0},
        {"15_Joint_Shoulder_Pitch_Left",  -0.2036},
        {"16_Joint_Shoulder_Roll_Left",    0.6879},
        {"17_Joint_Shoulder_Yaw_Left",    -0.9650},
        {"18_Joint_Elbow_Pitch_Left",     -1.7897},
        {"19_Joint_Wrist_Yaw_Left",       -0.8282},
        {"20_Joint_Wrist_Roll_Left",       0.0},
        {"21_Joint_Wrist_Pitch_Left",      0.0},
        {"22_Joint_Shoulder_Pitch_Right", -0.7655},
        {"23_Joint_Shoulder_Roll_Right",  -0.6274},
        {"24_Joint_Shoulder_Yaw_Right",    0.7999},
        {"25_Joint_Elbow_Pitch_Right",    -1.7549},
        {"26_Joint_Wrist_Yaw_Right",       0.5483},
        {"27_Joint_Wrist_Roll_Right",      0.0},
        {"28_Joint_Wrist_Pitch_Right",     0.0},
    };

    int repeat = this->get_parameter_or<int>("repeat", 3);
    int success_count = 0;
    int fail_count = 0;

    for (int i = 0; i < repeat && rclcpp::ok(); ++i) {
      RCLCPP_INFO(this->get_logger(), "=== [%d/%d] 목표 자세 1로 이동 ===", i + 1, repeat);
      if (plan_and_execute(both_arms, target1)) {
        success_count++;
      } else {
        fail_count++;
      }

      RCLCPP_INFO(this->get_logger(), "=== 2초 대기 ===");
      rclcpp::sleep_for(std::chrono::seconds(2));

      RCLCPP_INFO(this->get_logger(), "=== [%d/%d] 목표 자세 2로 이동 ===", i + 1, repeat);
      if (plan_and_execute(both_arms, target2)) {
        success_count++;
      } else {
        fail_count++;
      }

      RCLCPP_INFO(this->get_logger(), "=== 2초 대기 ===");
      rclcpp::sleep_for(std::chrono::seconds(2));

      RCLCPP_INFO(this->get_logger(),
          "[중간 결과] 성공: %d, 실패: %d (총 %d/%d)",
          success_count, fail_count, success_count + fail_count, repeat * 2);
    }

    RCLCPP_INFO(this->get_logger(),
        "=== 최종 결과: %d회 반복, 성공: %d, 실패: %d, 성공률: %.1f%% ===",
        repeat, success_count, fail_count,
        (success_count + fail_count > 0)
            ? 100.0 * success_count / (success_count + fail_count) : 0.0);
  }

private:
  bool plan_and_execute(
      moveit::planning_interface::MoveGroupInterface & group,
      const std::map<std::string, double> & joint_values,
      int max_retries = 2)
  {
    group.setJointValueTarget(joint_values);

    for (int attempt = 1; attempt <= max_retries; ++attempt) {
      moveit::planning_interface::MoveGroupInterface::Plan plan;
      auto result = group.plan(plan);
      if (result == moveit::core::MoveItErrorCode::SUCCESS) {
        RCLCPP_INFO(this->get_logger(),
            "양팔 플래닝 성공 (attempt: %d, points: %zu) - 실행합니다.",
            attempt, plan.trajectory.joint_trajectory.points.size());
        group.execute(plan);
        return true;
      }
      RCLCPP_WARN(this->get_logger(),
          "양팔 플래닝 실패 (attempt: %d/%d)", attempt, max_retries);
    }
    return false;
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SelfCollisionTestNode>();
  node->run();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
