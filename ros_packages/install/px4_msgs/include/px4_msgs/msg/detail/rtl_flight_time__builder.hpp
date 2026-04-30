// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from px4_msgs:msg/RtlFlightTime.idl
// generated code does not contain a copyright notice

#ifndef PX4_MSGS__MSG__DETAIL__RTL_FLIGHT_TIME__BUILDER_HPP_
#define PX4_MSGS__MSG__DETAIL__RTL_FLIGHT_TIME__BUILDER_HPP_

#include "px4_msgs/msg/detail/rtl_flight_time__struct.hpp"
#include <rosidl_runtime_cpp/message_initialization.hpp>
#include <algorithm>
#include <utility>


namespace px4_msgs
{

namespace msg
{

namespace builder
{

class Init_RtlFlightTime_rtl_limit_fraction
{
public:
  explicit Init_RtlFlightTime_rtl_limit_fraction(::px4_msgs::msg::RtlFlightTime & msg)
  : msg_(msg)
  {}
  ::px4_msgs::msg::RtlFlightTime rtl_limit_fraction(::px4_msgs::msg::RtlFlightTime::_rtl_limit_fraction_type arg)
  {
    msg_.rtl_limit_fraction = std::move(arg);
    return std::move(msg_);
  }

private:
  ::px4_msgs::msg::RtlFlightTime msg_;
};

class Init_RtlFlightTime_rtl_time_s
{
public:
  explicit Init_RtlFlightTime_rtl_time_s(::px4_msgs::msg::RtlFlightTime & msg)
  : msg_(msg)
  {}
  Init_RtlFlightTime_rtl_limit_fraction rtl_time_s(::px4_msgs::msg::RtlFlightTime::_rtl_time_s_type arg)
  {
    msg_.rtl_time_s = std::move(arg);
    return Init_RtlFlightTime_rtl_limit_fraction(msg_);
  }

private:
  ::px4_msgs::msg::RtlFlightTime msg_;
};

class Init_RtlFlightTime_timestamp
{
public:
  Init_RtlFlightTime_timestamp()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RtlFlightTime_rtl_time_s timestamp(::px4_msgs::msg::RtlFlightTime::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return Init_RtlFlightTime_rtl_time_s(msg_);
  }

private:
  ::px4_msgs::msg::RtlFlightTime msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::px4_msgs::msg::RtlFlightTime>()
{
  return px4_msgs::msg::builder::Init_RtlFlightTime_timestamp();
}

}  // namespace px4_msgs

#endif  // PX4_MSGS__MSG__DETAIL__RTL_FLIGHT_TIME__BUILDER_HPP_
