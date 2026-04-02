#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.hpp>
#include <moveit_msgs/srv/servo_command_type.hpp>
#include <std_srvs/srv/trigger.hpp>

using namespace std::chrono_literals;

class GoHomeNode : public rclcpp::Node
{
public:
  GoHomeNode() : Node("go_home_node")
  {
    // 서비스: ~/go_home 호출하면 자동으로 home 복귀
    service_ = this->create_service<std_srvs::srv::Trigger>(
      "~/go_home",
      std::bind(&GoHomeNode::goHomeCallback, this,
                std::placeholders::_1, std::placeholders::_2));

    // Servo switch_command_type 클라이언트들
    servo_groups_ = {
      {"left_arm",  "/left_servo/left_servo/switch_command_type"},
      {"right_arm", "/right_servo/right_servo/switch_command_type"},
      {"head",      "/head_servo/head_servo/switch_command_type"},
      {"waist",     "/waist_servo/waist_servo/switch_command_type"},
      {"left_leg",  "/left_leg_servo/left_leg_servo/switch_command_type"},
      {"right_leg", "/right_leg_servo/right_leg_servo/switch_command_type"},
    };

    for (auto & [group, service_name] : servo_groups_)
    {
      servo_clients_[group] =
        this->create_client<moveit_msgs::srv::ServoCommandType>(service_name);
    }

    RCLCPP_INFO(this->get_logger(), "GoHome node ready. Call ~/go_home service to return to home pose.");
  }

private:
  struct ServoGroup {
    std::string group_name;
    std::string service_name;
  };

  void switchServoCommandType(int command_type)
  {
    for (auto & [group, client] : servo_clients_)
    {
      if (!client->service_is_ready()) {
        RCLCPP_WARN(this->get_logger(), "Servo service not ready for %s, skipping", group.c_str());
        continue;
      }
      auto req = std::make_shared<moveit_msgs::srv::ServoCommandType::Request>();
      req->command_type = command_type;
      auto future = client->async_send_request(req);
      // 짧게 대기
      if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future, 2s)
          == rclcpp::FutureReturnCode::SUCCESS)
      {
        RCLCPP_INFO(this->get_logger(), "Servo %s command_type set to %d", group.c_str(), command_type);
      }
    }
  }

  bool moveGroupToHome(const std::string & group_name)
  {
    try {
      auto move_group = moveit::planning_interface::MoveGroupInterface(
        this->shared_from_this(), group_name);
      move_group.setNamedTarget("home");
      move_group.setMaxVelocityScalingFactor(0.3);
      move_group.setMaxAccelerationScalingFactor(0.3);

      auto result = move_group.move();
      if (result == moveit::core::MoveItErrorCode::SUCCESS) {
        RCLCPP_INFO(this->get_logger(), "%s moved to home", group_name.c_str());
        return true;
      } else {
        RCLCPP_ERROR(this->get_logger(), "%s failed to move to home", group_name.c_str());
        return false;
      }
    } catch (const std::exception & e) {
      RCLCPP_ERROR(this->get_logger(), "%s error: %s", group_name.c_str(), e.what());
      return false;
    }
  }

  void goHomeCallback(
    const std::shared_ptr<std_srvs::srv::Trigger::Request>,
    std::shared_ptr<std_srvs::srv::Trigger::Response> response)
  {
    RCLCPP_INFO(this->get_logger(), "Go home requested - disabling servos...");

    // 1. 모든 Servo 비활성화 (command_type = 0: NONE)
    switchServoCommandType(0);

    // 2. 잠시 대기 - Servo가 완전히 멈추도록
    rclcpp::sleep_for(500ms);

    // 3. 각 그룹을 home으로 이동
    bool all_ok = true;
    std::vector<std::string> groups = {
      "left_arm", "right_arm", "head", "waist", "left_leg", "right_leg"
    };
    for (auto & g : groups) {
      if (!moveGroupToHome(g)) {
        all_ok = false;
      }
    }

    // 4. Servo 재활성화 (command_type = 1: TWIST)
    RCLCPP_INFO(this->get_logger(), "Re-enabling servos...");
    switchServoCommandType(1);

    response->success = all_ok;
    response->message = all_ok ? "All groups returned to home" : "Some groups failed";
  }

  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr service_;
  std::vector<ServoGroup> servo_groups_;
  std::map<std::string, rclcpp::Client<moveit_msgs::srv::ServoCommandType>::SharedPtr> servo_clients_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<GoHomeNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
