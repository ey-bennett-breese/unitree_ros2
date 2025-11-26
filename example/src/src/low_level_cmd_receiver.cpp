#include "rclcpp/rclcpp.hpp"
#include "unitree_go/msg/low_cmd.hpp"

class LowLevelCmdReceiver : public rclcpp::Node {
public:
  LowLevelCmdReceiver() : Node("low_level_cmd_receiver") {
    // Create the subscriber to listen to the "/lowcmd" topic
    cmd_subscriber_ = this->create_subscription<unitree_go::msg::LowCmd>(
      "/lowcmd", 10, std::bind(&LowLevelCmdReceiver::cmd_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "LowLevelCmdReceiver is now listening to /lowcmd topic...");
  }

private:
  void cmd_callback(const unitree_go::msg::LowCmd::SharedPtr msg) {
    RCLCPP_INFO(this->get_logger(), "Received LowCmd message!");

    // Here you can process the received message
    for (size_t i = 0; i < msg->motor_cmd.size(); i++) {
      RCLCPP_INFO(this->get_logger(), "MotorCmd[%zu]: q = %.2f, tau = %.2f",
                  i, msg->motor_cmd[i].q, msg->motor_cmd[i].tau);
    }
  }

  rclcpp::Subscription<unitree_go::msg::LowCmd>::SharedPtr cmd_subscriber_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);  // Initialize ROS 2
  auto node = std::make_shared<LowLevelCmdReceiver>();  // Create the subscriber node
  rclcpp::spin(node);  // Keep the node alive and processing callbacks
  rclcpp::shutdown();  // Shut down ROS 2 when done
  return 0;
}
