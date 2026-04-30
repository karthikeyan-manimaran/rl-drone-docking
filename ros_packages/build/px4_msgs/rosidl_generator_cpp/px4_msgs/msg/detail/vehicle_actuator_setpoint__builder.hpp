// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from px4_msgs:msg/VehicleActuatorSetpoint.idl
// generated code does not contain a copyright notice

#ifndef PX4_MSGS__MSG__DETAIL__VEHICLE_ACTUATOR_SETPOINT__BUILDER_HPP_
#define PX4_MSGS__MSG__DETAIL__VEHICLE_ACTUATOR_SETPOINT__BUILDER_HPP_

#include "px4_msgs/msg/detail/vehicle_actuator_setpoint__struct.hpp"
#include <rosidl_runtime_cpp/message_initialization.hpp>
#include <algorithm>
#include <utility>


namespace px4_msgs
{

namespace msg
{

namespace builder
{

class Init_VehicleActuatorSetpoint_actuator
{
public:
  explicit Init_VehicleActuatorSetpoint_actuator(::px4_msgs::msg::VehicleActuatorSetpoint & msg)
  : msg_(msg)
  {}
  ::px4_msgs::msg::VehicleActuatorSetpoint actuator(::px4_msgs::msg::VehicleActuatorSetpoint::_actuator_type arg)
  {
    msg_.actuator = std::move(arg);
    return std::move(msg_);
  }

private:
  ::px4_msgs::msg::VehicleActuatorSetpoint msg_;
};

class Init_VehicleActuatorSetpoint_timestamp_sample
{
public:
  explicit Init_VehicleActuatorSetpoint_timestamp_sample(::px4_msgs::msg::VehicleActuatorSetpoint & msg)
  : msg_(msg)
  {}
  Init_VehicleActuatorSetpoint_actuator timestamp_sample(::px4_msgs::msg::VehicleActuatorSetpoint::_timestamp_sample_type arg)
  {
    msg_.timestamp_sample = std::move(arg);
    return Init_VehicleActuatorSetpoint_actuator(msg_);
  }

private:
  ::px4_msgs::msg::VehicleActuatorSetpoint msg_;
};

class Init_VehicleActuatorSetpoint_timestamp
{
public:
  Init_VehicleActuatorSetpoint_timestamp()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_VehicleActuatorSetpoint_timestamp_sample timestamp(::px4_msgs::msg::VehicleActuatorSetpoint::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return Init_VehicleActuatorSetpoint_timestamp_sample(msg_);
  }

private:
  ::px4_msgs::msg::VehicleActuatorSetpoint msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::px4_msgs::msg::VehicleActuatorSetpoint>()
{
  return px4_msgs::msg::builder::Init_VehicleActuatorSetpoint_timestamp();
}

}  // namespace px4_msgs

#endif  // PX4_MSGS__MSG__DETAIL__VEHICLE_ACTUATOR_SETPOINT__BUILDER_HPP_
