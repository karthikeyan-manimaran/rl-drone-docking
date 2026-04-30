// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from px4_msgs:msg/EkfGpsDrift.idl
// generated code does not contain a copyright notice

#ifndef PX4_MSGS__MSG__DETAIL__EKF_GPS_DRIFT__BUILDER_HPP_
#define PX4_MSGS__MSG__DETAIL__EKF_GPS_DRIFT__BUILDER_HPP_

#include "px4_msgs/msg/detail/ekf_gps_drift__struct.hpp"
#include <rosidl_runtime_cpp/message_initialization.hpp>
#include <algorithm>
#include <utility>


namespace px4_msgs
{

namespace msg
{

namespace builder
{

class Init_EkfGpsDrift_blocked
{
public:
  explicit Init_EkfGpsDrift_blocked(::px4_msgs::msg::EkfGpsDrift & msg)
  : msg_(msg)
  {}
  ::px4_msgs::msg::EkfGpsDrift blocked(::px4_msgs::msg::EkfGpsDrift::_blocked_type arg)
  {
    msg_.blocked = std::move(arg);
    return std::move(msg_);
  }

private:
  ::px4_msgs::msg::EkfGpsDrift msg_;
};

class Init_EkfGpsDrift_hspd
{
public:
  explicit Init_EkfGpsDrift_hspd(::px4_msgs::msg::EkfGpsDrift & msg)
  : msg_(msg)
  {}
  Init_EkfGpsDrift_blocked hspd(::px4_msgs::msg::EkfGpsDrift::_hspd_type arg)
  {
    msg_.hspd = std::move(arg);
    return Init_EkfGpsDrift_blocked(msg_);
  }

private:
  ::px4_msgs::msg::EkfGpsDrift msg_;
};

class Init_EkfGpsDrift_vpos_drift_rate
{
public:
  explicit Init_EkfGpsDrift_vpos_drift_rate(::px4_msgs::msg::EkfGpsDrift & msg)
  : msg_(msg)
  {}
  Init_EkfGpsDrift_hspd vpos_drift_rate(::px4_msgs::msg::EkfGpsDrift::_vpos_drift_rate_type arg)
  {
    msg_.vpos_drift_rate = std::move(arg);
    return Init_EkfGpsDrift_hspd(msg_);
  }

private:
  ::px4_msgs::msg::EkfGpsDrift msg_;
};

class Init_EkfGpsDrift_hpos_drift_rate
{
public:
  explicit Init_EkfGpsDrift_hpos_drift_rate(::px4_msgs::msg::EkfGpsDrift & msg)
  : msg_(msg)
  {}
  Init_EkfGpsDrift_vpos_drift_rate hpos_drift_rate(::px4_msgs::msg::EkfGpsDrift::_hpos_drift_rate_type arg)
  {
    msg_.hpos_drift_rate = std::move(arg);
    return Init_EkfGpsDrift_vpos_drift_rate(msg_);
  }

private:
  ::px4_msgs::msg::EkfGpsDrift msg_;
};

class Init_EkfGpsDrift_timestamp
{
public:
  Init_EkfGpsDrift_timestamp()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_EkfGpsDrift_hpos_drift_rate timestamp(::px4_msgs::msg::EkfGpsDrift::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return Init_EkfGpsDrift_hpos_drift_rate(msg_);
  }

private:
  ::px4_msgs::msg::EkfGpsDrift msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::px4_msgs::msg::EkfGpsDrift>()
{
  return px4_msgs::msg::builder::Init_EkfGpsDrift_timestamp();
}

}  // namespace px4_msgs

#endif  // PX4_MSGS__MSG__DETAIL__EKF_GPS_DRIFT__BUILDER_HPP_
