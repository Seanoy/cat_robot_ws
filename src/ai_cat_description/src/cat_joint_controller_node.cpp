#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <chrono>
#include <cmath>

using namespace std::chrono_literals;

class CatMotionPublisher : public rclcpp::Node
{
public:
  CatMotionPublisher() : Node("cat_motion_publisher"), count_(0)
  {
    pub_ = this->create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);
    timer_ = this->create_wall_timer(50ms, std::bind(&CatMotionPublisher::timer_callback, this));

    msg_.name = {
      "lf_shoulder_pitch", "lf_elbow_pitch",
      "rf_shoulder_pitch", "rf_elbow_pitch",
      "neck_pitch", "neck_yaw",
      "tail_base"
    };
    msg_.position.resize(7, 0.0);
  }

private:
  void timer_callback()
  {
    double t = this->now().seconds();

    // 模拟踩奶动作（两个前爪交替）
    double phase = std::sin(t * 4.0) * 0.4;
    msg_.position[0] =  phase;       // lf_shoulder
    msg_.position[1] = -0.6 + phase*0.8;  // lf_elbow 向下压
    msg_.position[2] = -phase;       // rf_shoulder 反相
    msg_.position[3] = -0.6 - phase*0.8;

    // 头部左右看
    msg_.position[4] = std::sin(t * 0.6) * 0.3;     // pitch 小幅度点头
    msg_.position[5] = std::sin(t * 0.9) * 0.7;     // yaw 看左右

    // 尾巴甩动
    msg_.position[6] = std::sin(t * 5.0) * 0.8;

    msg_.header.stamp = this->now();
    pub_->publish(msg_);
  }

  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
  sensor_msgs::msg::JointState msg_;
  size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CatMotionPublisher>());
  rclcpp::shutdown();
  return 0;
}