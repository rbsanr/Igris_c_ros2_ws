// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from interface:srv/MoveJoints.idl
// generated code does not contain a copyright notice

#ifndef INTERFACE__SRV__DETAIL__MOVE_JOINTS__STRUCT_H_
#define INTERFACE__SRV__DETAIL__MOVE_JOINTS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'ids'
// Member 'positions'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in srv/MoveJoints in the package interface.
typedef struct interface__srv__MoveJoints_Request
{
  /// <--- string[] names 에서 변경됨
  rosidl_runtime_c__int32__Sequence ids;
  /// 목표 위치 (Radian)
  rosidl_runtime_c__double__Sequence positions;
  /// 이동 속도
  double velocity;
} interface__srv__MoveJoints_Request;

// Struct for a sequence of interface__srv__MoveJoints_Request.
typedef struct interface__srv__MoveJoints_Request__Sequence
{
  interface__srv__MoveJoints_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} interface__srv__MoveJoints_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'message'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/MoveJoints in the package interface.
typedef struct interface__srv__MoveJoints_Response
{
  bool success;
  rosidl_runtime_c__String message;
} interface__srv__MoveJoints_Response;

// Struct for a sequence of interface__srv__MoveJoints_Response.
typedef struct interface__srv__MoveJoints_Response__Sequence
{
  interface__srv__MoveJoints_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} interface__srv__MoveJoints_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // INTERFACE__SRV__DETAIL__MOVE_JOINTS__STRUCT_H_
