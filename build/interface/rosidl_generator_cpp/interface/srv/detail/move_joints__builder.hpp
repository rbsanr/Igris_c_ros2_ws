// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from interface:srv/MoveJoints.idl
// generated code does not contain a copyright notice

#ifndef INTERFACE__SRV__DETAIL__MOVE_JOINTS__BUILDER_HPP_
#define INTERFACE__SRV__DETAIL__MOVE_JOINTS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "interface/srv/detail/move_joints__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace interface
{

namespace srv
{

namespace builder
{

class Init_MoveJoints_Request_velocity
{
public:
  explicit Init_MoveJoints_Request_velocity(::interface::srv::MoveJoints_Request & msg)
  : msg_(msg)
  {}
  ::interface::srv::MoveJoints_Request velocity(::interface::srv::MoveJoints_Request::_velocity_type arg)
  {
    msg_.velocity = std::move(arg);
    return std::move(msg_);
  }

private:
  ::interface::srv::MoveJoints_Request msg_;
};

class Init_MoveJoints_Request_positions
{
public:
  explicit Init_MoveJoints_Request_positions(::interface::srv::MoveJoints_Request & msg)
  : msg_(msg)
  {}
  Init_MoveJoints_Request_velocity positions(::interface::srv::MoveJoints_Request::_positions_type arg)
  {
    msg_.positions = std::move(arg);
    return Init_MoveJoints_Request_velocity(msg_);
  }

private:
  ::interface::srv::MoveJoints_Request msg_;
};

class Init_MoveJoints_Request_ids
{
public:
  Init_MoveJoints_Request_ids()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoveJoints_Request_positions ids(::interface::srv::MoveJoints_Request::_ids_type arg)
  {
    msg_.ids = std::move(arg);
    return Init_MoveJoints_Request_positions(msg_);
  }

private:
  ::interface::srv::MoveJoints_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::interface::srv::MoveJoints_Request>()
{
  return interface::srv::builder::Init_MoveJoints_Request_ids();
}

}  // namespace interface


namespace interface
{

namespace srv
{

namespace builder
{

class Init_MoveJoints_Response_message
{
public:
  explicit Init_MoveJoints_Response_message(::interface::srv::MoveJoints_Response & msg)
  : msg_(msg)
  {}
  ::interface::srv::MoveJoints_Response message(::interface::srv::MoveJoints_Response::_message_type arg)
  {
    msg_.message = std::move(arg);
    return std::move(msg_);
  }

private:
  ::interface::srv::MoveJoints_Response msg_;
};

class Init_MoveJoints_Response_success
{
public:
  Init_MoveJoints_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoveJoints_Response_message success(::interface::srv::MoveJoints_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_MoveJoints_Response_message(msg_);
  }

private:
  ::interface::srv::MoveJoints_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::interface::srv::MoveJoints_Response>()
{
  return interface::srv::builder::Init_MoveJoints_Response_success();
}

}  // namespace interface

#endif  // INTERFACE__SRV__DETAIL__MOVE_JOINTS__BUILDER_HPP_
