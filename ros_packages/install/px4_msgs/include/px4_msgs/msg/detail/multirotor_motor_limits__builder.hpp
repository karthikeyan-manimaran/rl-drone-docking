// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from px4_msgs:msg/MultirotorMotorLimits.idl
// generated code does not contain a copyright notice

#ifndef PX4_MSGS__MSG__DETAIL__MULTIROTOR_MOTOR_LIMITS__BUILDER_HPP_
#define PX4_MSGS__MSG__DETAIL__MULTIROTOR_MOTOR_LIMITS__BUILDER_HPP_

#include "px4_msgs/msg/detail/multirotor_motor_limits__struct.hpp"
#include <rosidl_runtime_cpp/message_initialization.hpp>
#include <algorithm>
#include <utility>


namespace px4_msgs
{

namespace msg
{

namespace builder
{

class Init_MultirotorMotorLimits_saturation_status
{
public:
  explicit Init_MultirotorMotorLimits_saturation_status(::px4_msgs::msg::MultirotorMotorLimits & msg)
  : msg_(msg)
  {}
  ::px4_msgs::msg::MultirotorMotorLimits saturation_status(::px4_msgs::msg::MultirotorMotorLimits::_saturation_status_type arg)
  {
    msg_.saturation_status = std::move(arg);
    return std::move(msg_);
  }

private:
  ::px4_msgs::msg::MultirotorMotorLimits msg_;
};

class Init_MultirotorMotorLimits_timestamp
{
public:
  Init_MultirotorMotorLimits_timestamp()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MultirotorMotorLimits_saturation_status timestamp(::px4_msgs::msg::MultirotorMotorLimits::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return Init_MultirotorMotorLimits_saturation_status(msg_);
  }

private:
  ::px4_msgs::msg::MultirotorMotorLimits msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::px4_msgs::msg::MultirotorMotorLimits>()
{
  return px4_msgs::msg::builder::Init_MultirotorMotorLimits_timestamp();
}

}  // namespace px4_msgs

#endif  // PX4_MSGS__MSG__DETAIL__MULTIROTOR_MOTOR_LIMITS__BUILDER_HPP_
