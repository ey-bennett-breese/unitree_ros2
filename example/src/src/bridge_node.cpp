#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <unitree_go/msg/low_state.hpp>
#include <unitree_go/msg/low_cmd.hpp>

class BridgeNode : public rclcpp::Node {
public:
  BridgeNode() : Node("bridge_node") {
    // Subscriber for /joint_states -> Publisher for /lowstate
    joint_states_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
        "/joint_states", 10, std::bind(&BridgeNode::jointStatesCallback, this, std::placeholders::_1));
    lowstate_pub_ = this->create_publisher<unitree_go::msg::LowState>("/lowstate", 10);

    // Subscriber for /lowcmd -> Publisher for /joint_command
    lowcmd_sub_ = this->create_subscription<unitree_go::msg::LowCmd>(
        "/lowcmd", 10, std::bind(&BridgeNode::lowCmdCallback, this, std::placeholders::_1));
    joint_command_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("/joint_command", 10);

    // Joint names in the order from /joint_states
    joint_names_ = {
        "FL_hip_joint", "FR_hip_joint", "RL_hip_joint", "RR_hip_joint",
        "FL_thigh_joint", "FR_thigh_joint", "RL_thigh_joint", "RR_thigh_joint",
        "FL_calf_joint", "FR_calf_joint", "RL_calf_joint", "RR_calf_joint"
    };

    // Mapping from joint_states index to Unitree motor index
    joint_to_motor_ = {3, 0, 9, 6, 4, 1, 10, 7, 5, 2, 11, 8};
  }

private:
  void jointStatesCallback(const sensor_msgs::msg::JointState::SharedPtr msg) {
    unitree_go::msg::LowState lowstate;
    // Initialize motor states to defaults
    for (int i = 0; i < 20; ++i) {
      lowstate.motor_state[i].mode = 0;
      lowstate.motor_state[i].q = 0.0f;
      lowstate.motor_state[i].dq = 0.0f;
      lowstate.motor_state[i].ddq = 0.0f;
      lowstate.motor_state[i].tau_est = 0.0f;
      lowstate.motor_state[i].temperature = 0;
      lowstate.motor_state[i].reserve[0] = 0;
      lowstate.motor_state[i].reserve[1] = 0;
    }
    // Map the 12 joints
    if (msg->name.size() == 12 && msg->position.size() == 12 && msg->velocity.size() == 12 && msg->effort.size() == 12) {
      for (int j = 0; j < 12; ++j) {
        int motor_idx = joint_to_motor_[j];
        lowstate.motor_state[motor_idx].q = static_cast<float>(msg->position[j]);
        lowstate.motor_state[motor_idx].dq = static_cast<float>(msg->velocity[j]);
        lowstate.motor_state[motor_idx].tau_est = static_cast<float>(msg->effort[j]);
        lowstate.motor_state[motor_idx].mode = 1; // Assume servo mode
      }
    }
    // Other fields like IMU, foot_force left as default (zeros)
    lowstate_pub_->publish(lowstate);
  }

  void lowCmdCallback(const unitree_go::msg::LowCmd::SharedPtr msg) {
    sensor_msgs::msg::JointState joint_cmd;
    joint_cmd.header.stamp = this->now();
    joint_cmd.name = joint_names_;
    joint_cmd.position.resize(12);
    joint_cmd.velocity.resize(12);
    joint_cmd.effort.resize(12);
    for (int j = 0; j < 12; ++j) {
      int motor_idx = joint_to_motor_[j];
      joint_cmd.position[j] = static_cast<double>(msg->motor_cmd[motor_idx].q);
      joint_cmd.velocity[j] = static_cast<double>(msg->motor_cmd[motor_idx].dq);
      joint_cmd.effort[j] = static_cast<double>(msg->motor_cmd[motor_idx].tau);
    }
    joint_command_pub_->publish(joint_cmd);
  }

  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_states_sub_;
  rclcpp::Publisher<unitree_go::msg::LowState>::SharedPtr lowstate_pub_;
  rclcpp::Subscription<unitree_go::msg::LowCmd>::SharedPtr lowcmd_sub_;
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_command_pub_;
  std::vector<std::string> joint_names_;
  std::vector<int> joint_to_motor_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BridgeNode>());
  rclcpp::shutdown();
  return 0;
}