// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from interface:srv/MoveJoints.idl
// generated code does not contain a copyright notice

#ifndef INTERFACE__SRV__DETAIL__MOVE_JOINTS__TRAITS_HPP_
#define INTERFACE__SRV__DETAIL__MOVE_JOINTS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "interface/srv/detail/move_joints__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace interface
{

namespace srv
{

inline void to_flow_style_yaml(
  const MoveJoints_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: ids
  {
    if (msg.ids.size() == 0) {
      out << "ids: []";
    } else {
      out << "ids: [";
      size_t pending_items = msg.ids.size();
      for (auto item : msg.ids) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: positions
  {
    if (msg.positions.size() == 0) {
      out << "positions: []";
    } else {
      out << "positions: [";
      size_t pending_items = msg.positions.size();
      for (auto item : msg.positions) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: velocity
  {
    out << "velocity: ";
    rosidl_generator_traits::value_to_yaml(msg.velocity, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const MoveJoints_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: ids
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.ids.size() == 0) {
      out << "ids: []\n";
    } else {
      out << "ids:\n";
      for (auto item : msg.ids) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: positions
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.positions.size() == 0) {
      out << "positions: []\n";
    } else {
      out << "positions:\n";
      for (auto item : msg.positions) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: velocity
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "velocity: ";
    rosidl_generator_traits::value_to_yaml(msg.velocity, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const MoveJoints_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace interface

namespace rosidl_generator_traits
{

[[deprecated("use interface::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const interface::srv::MoveJoints_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  interface::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use interface::srv::to_yaml() instead")]]
inline std::string to_yaml(const interface::srv::MoveJoints_Request & msg)
{
  return interface::srv::to_yaml(msg);
}

template<>
inline const char * data_type<interface::srv::MoveJoints_Request>()
{
  return "interface::srv::MoveJoints_Request";
}

template<>
inline const char * name<interface::srv::MoveJoints_Request>()
{
  return "interface/srv/MoveJoints_Request";
}

template<>
struct has_fixed_size<interface::srv::MoveJoints_Request>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<interface::srv::MoveJoints_Request>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<interface::srv::MoveJoints_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace interface
{

namespace srv
{

inline void to_flow_style_yaml(
  const MoveJoints_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: success
  {
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << ", ";
  }

  // member: message
  {
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const MoveJoints_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: success
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << "\n";
  }

  // member: message
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const MoveJoints_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace interface

namespace rosidl_generator_traits
{

[[deprecated("use interface::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const interface::srv::MoveJoints_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  interface::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use interface::srv::to_yaml() instead")]]
inline std::string to_yaml(const interface::srv::MoveJoints_Response & msg)
{
  return interface::srv::to_yaml(msg);
}

template<>
inline const char * data_type<interface::srv::MoveJoints_Response>()
{
  return "interface::srv::MoveJoints_Response";
}

template<>
inline const char * name<interface::srv::MoveJoints_Response>()
{
  return "interface/srv/MoveJoints_Response";
}

template<>
struct has_fixed_size<interface::srv::MoveJoints_Response>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<interface::srv::MoveJoints_Response>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<interface::srv::MoveJoints_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<interface::srv::MoveJoints>()
{
  return "interface::srv::MoveJoints";
}

template<>
inline const char * name<interface::srv::MoveJoints>()
{
  return "interface/srv/MoveJoints";
}

template<>
struct has_fixed_size<interface::srv::MoveJoints>
  : std::integral_constant<
    bool,
    has_fixed_size<interface::srv::MoveJoints_Request>::value &&
    has_fixed_size<interface::srv::MoveJoints_Response>::value
  >
{
};

template<>
struct has_bounded_size<interface::srv::MoveJoints>
  : std::integral_constant<
    bool,
    has_bounded_size<interface::srv::MoveJoints_Request>::value &&
    has_bounded_size<interface::srv::MoveJoints_Response>::value
  >
{
};

template<>
struct is_service<interface::srv::MoveJoints>
  : std::true_type
{
};

template<>
struct is_service_request<interface::srv::MoveJoints_Request>
  : std::true_type
{
};

template<>
struct is_service_response<interface::srv::MoveJoints_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // INTERFACE__SRV__DETAIL__MOVE_JOINTS__TRAITS_HPP_
