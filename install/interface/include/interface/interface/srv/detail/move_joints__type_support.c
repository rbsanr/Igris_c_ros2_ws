// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from interface:srv/MoveJoints.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "interface/srv/detail/move_joints__rosidl_typesupport_introspection_c.h"
#include "interface/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "interface/srv/detail/move_joints__functions.h"
#include "interface/srv/detail/move_joints__struct.h"


// Include directives for member types
// Member `ids`
// Member `positions`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  interface__srv__MoveJoints_Request__init(message_memory);
}

void interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_fini_function(void * message_memory)
{
  interface__srv__MoveJoints_Request__fini(message_memory);
}

size_t interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__size_function__MoveJoints_Request__ids(
  const void * untyped_member)
{
  const rosidl_runtime_c__int32__Sequence * member =
    (const rosidl_runtime_c__int32__Sequence *)(untyped_member);
  return member->size;
}

const void * interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_const_function__MoveJoints_Request__ids(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__int32__Sequence * member =
    (const rosidl_runtime_c__int32__Sequence *)(untyped_member);
  return &member->data[index];
}

void * interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_function__MoveJoints_Request__ids(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__int32__Sequence * member =
    (rosidl_runtime_c__int32__Sequence *)(untyped_member);
  return &member->data[index];
}

void interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__fetch_function__MoveJoints_Request__ids(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int32_t * item =
    ((const int32_t *)
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_const_function__MoveJoints_Request__ids(untyped_member, index));
  int32_t * value =
    (int32_t *)(untyped_value);
  *value = *item;
}

void interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__assign_function__MoveJoints_Request__ids(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int32_t * item =
    ((int32_t *)
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_function__MoveJoints_Request__ids(untyped_member, index));
  const int32_t * value =
    (const int32_t *)(untyped_value);
  *item = *value;
}

bool interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__resize_function__MoveJoints_Request__ids(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__int32__Sequence * member =
    (rosidl_runtime_c__int32__Sequence *)(untyped_member);
  rosidl_runtime_c__int32__Sequence__fini(member);
  return rosidl_runtime_c__int32__Sequence__init(member, size);
}

size_t interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__size_function__MoveJoints_Request__positions(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_const_function__MoveJoints_Request__positions(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_function__MoveJoints_Request__positions(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__fetch_function__MoveJoints_Request__positions(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_const_function__MoveJoints_Request__positions(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__assign_function__MoveJoints_Request__positions(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_function__MoveJoints_Request__positions(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__resize_function__MoveJoints_Request__positions(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_message_member_array[3] = {
  {
    "ids",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(interface__srv__MoveJoints_Request, ids),  // bytes offset in struct
    NULL,  // default value
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__size_function__MoveJoints_Request__ids,  // size() function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_const_function__MoveJoints_Request__ids,  // get_const(index) function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_function__MoveJoints_Request__ids,  // get(index) function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__fetch_function__MoveJoints_Request__ids,  // fetch(index, &value) function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__assign_function__MoveJoints_Request__ids,  // assign(index, value) function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__resize_function__MoveJoints_Request__ids  // resize(index) function pointer
  },
  {
    "positions",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(interface__srv__MoveJoints_Request, positions),  // bytes offset in struct
    NULL,  // default value
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__size_function__MoveJoints_Request__positions,  // size() function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_const_function__MoveJoints_Request__positions,  // get_const(index) function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__get_function__MoveJoints_Request__positions,  // get(index) function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__fetch_function__MoveJoints_Request__positions,  // fetch(index, &value) function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__assign_function__MoveJoints_Request__positions,  // assign(index, value) function pointer
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__resize_function__MoveJoints_Request__positions  // resize(index) function pointer
  },
  {
    "velocity",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(interface__srv__MoveJoints_Request, velocity),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_message_members = {
  "interface__srv",  // message namespace
  "MoveJoints_Request",  // message name
  3,  // number of fields
  sizeof(interface__srv__MoveJoints_Request),
  interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_message_member_array,  // message members
  interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_message_type_support_handle = {
  0,
  &interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_interface
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, interface, srv, MoveJoints_Request)() {
  if (!interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_message_type_support_handle.typesupport_identifier) {
    interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &interface__srv__MoveJoints_Request__rosidl_typesupport_introspection_c__MoveJoints_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "interface/srv/detail/move_joints__rosidl_typesupport_introspection_c.h"
// already included above
// #include "interface/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "interface/srv/detail/move_joints__functions.h"
// already included above
// #include "interface/srv/detail/move_joints__struct.h"


// Include directives for member types
// Member `message`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  interface__srv__MoveJoints_Response__init(message_memory);
}

void interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_fini_function(void * message_memory)
{
  interface__srv__MoveJoints_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_message_member_array[2] = {
  {
    "success",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(interface__srv__MoveJoints_Response, success),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "message",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(interface__srv__MoveJoints_Response, message),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_message_members = {
  "interface__srv",  // message namespace
  "MoveJoints_Response",  // message name
  2,  // number of fields
  sizeof(interface__srv__MoveJoints_Response),
  interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_message_member_array,  // message members
  interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_message_type_support_handle = {
  0,
  &interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_interface
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, interface, srv, MoveJoints_Response)() {
  if (!interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_message_type_support_handle.typesupport_identifier) {
    interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &interface__srv__MoveJoints_Response__rosidl_typesupport_introspection_c__MoveJoints_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "interface/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "interface/srv/detail/move_joints__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers interface__srv__detail__move_joints__rosidl_typesupport_introspection_c__MoveJoints_service_members = {
  "interface__srv",  // service namespace
  "MoveJoints",  // service name
  // these two fields are initialized below on the first access
  NULL,  // request message
  // interface__srv__detail__move_joints__rosidl_typesupport_introspection_c__MoveJoints_Request_message_type_support_handle,
  NULL  // response message
  // interface__srv__detail__move_joints__rosidl_typesupport_introspection_c__MoveJoints_Response_message_type_support_handle
};

static rosidl_service_type_support_t interface__srv__detail__move_joints__rosidl_typesupport_introspection_c__MoveJoints_service_type_support_handle = {
  0,
  &interface__srv__detail__move_joints__rosidl_typesupport_introspection_c__MoveJoints_service_members,
  get_service_typesupport_handle_function,
};

// Forward declaration of request/response type support functions
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, interface, srv, MoveJoints_Request)();

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, interface, srv, MoveJoints_Response)();

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_interface
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, interface, srv, MoveJoints)() {
  if (!interface__srv__detail__move_joints__rosidl_typesupport_introspection_c__MoveJoints_service_type_support_handle.typesupport_identifier) {
    interface__srv__detail__move_joints__rosidl_typesupport_introspection_c__MoveJoints_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)interface__srv__detail__move_joints__rosidl_typesupport_introspection_c__MoveJoints_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, interface, srv, MoveJoints_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, interface, srv, MoveJoints_Response)()->data;
  }

  return &interface__srv__detail__move_joints__rosidl_typesupport_introspection_c__MoveJoints_service_type_support_handle;
}
