// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from interface:srv/MoveJoints.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "interface/srv/detail/move_joints__functions.h"
#include "interface/srv/detail/move_joints__struct.hpp"
#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
#include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace interface
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoveJoints_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoveJoints_Request_type_support_ids_t;

static const _MoveJoints_Request_type_support_ids_t _MoveJoints_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoveJoints_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoveJoints_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoveJoints_Request_type_support_symbol_names_t _MoveJoints_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, interface, srv, MoveJoints_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, interface, srv, MoveJoints_Request)),
  }
};

typedef struct _MoveJoints_Request_type_support_data_t
{
  void * data[2];
} _MoveJoints_Request_type_support_data_t;

static _MoveJoints_Request_type_support_data_t _MoveJoints_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoveJoints_Request_message_typesupport_map = {
  2,
  "interface",
  &_MoveJoints_Request_message_typesupport_ids.typesupport_identifier[0],
  &_MoveJoints_Request_message_typesupport_symbol_names.symbol_name[0],
  &_MoveJoints_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoveJoints_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoveJoints_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &interface__srv__MoveJoints_Request__get_type_hash,
  &interface__srv__MoveJoints_Request__get_type_description,
  &interface__srv__MoveJoints_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace interface

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<interface::srv::MoveJoints_Request>()
{
  return &::interface::srv::rosidl_typesupport_cpp::MoveJoints_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, interface, srv, MoveJoints_Request)() {
  return get_message_type_support_handle<interface::srv::MoveJoints_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "interface/srv/detail/move_joints__functions.h"
// already included above
// #include "interface/srv/detail/move_joints__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace interface
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoveJoints_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoveJoints_Response_type_support_ids_t;

static const _MoveJoints_Response_type_support_ids_t _MoveJoints_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoveJoints_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoveJoints_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoveJoints_Response_type_support_symbol_names_t _MoveJoints_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, interface, srv, MoveJoints_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, interface, srv, MoveJoints_Response)),
  }
};

typedef struct _MoveJoints_Response_type_support_data_t
{
  void * data[2];
} _MoveJoints_Response_type_support_data_t;

static _MoveJoints_Response_type_support_data_t _MoveJoints_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoveJoints_Response_message_typesupport_map = {
  2,
  "interface",
  &_MoveJoints_Response_message_typesupport_ids.typesupport_identifier[0],
  &_MoveJoints_Response_message_typesupport_symbol_names.symbol_name[0],
  &_MoveJoints_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoveJoints_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoveJoints_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &interface__srv__MoveJoints_Response__get_type_hash,
  &interface__srv__MoveJoints_Response__get_type_description,
  &interface__srv__MoveJoints_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace interface

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<interface::srv::MoveJoints_Response>()
{
  return &::interface::srv::rosidl_typesupport_cpp::MoveJoints_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, interface, srv, MoveJoints_Response)() {
  return get_message_type_support_handle<interface::srv::MoveJoints_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "interface/srv/detail/move_joints__functions.h"
// already included above
// #include "interface/srv/detail/move_joints__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace interface
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoveJoints_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoveJoints_Event_type_support_ids_t;

static const _MoveJoints_Event_type_support_ids_t _MoveJoints_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoveJoints_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoveJoints_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoveJoints_Event_type_support_symbol_names_t _MoveJoints_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, interface, srv, MoveJoints_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, interface, srv, MoveJoints_Event)),
  }
};

typedef struct _MoveJoints_Event_type_support_data_t
{
  void * data[2];
} _MoveJoints_Event_type_support_data_t;

static _MoveJoints_Event_type_support_data_t _MoveJoints_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoveJoints_Event_message_typesupport_map = {
  2,
  "interface",
  &_MoveJoints_Event_message_typesupport_ids.typesupport_identifier[0],
  &_MoveJoints_Event_message_typesupport_symbol_names.symbol_name[0],
  &_MoveJoints_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoveJoints_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoveJoints_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &interface__srv__MoveJoints_Event__get_type_hash,
  &interface__srv__MoveJoints_Event__get_type_description,
  &interface__srv__MoveJoints_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace interface

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<interface::srv::MoveJoints_Event>()
{
  return &::interface::srv::rosidl_typesupport_cpp::MoveJoints_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, interface, srv, MoveJoints_Event)() {
  return get_message_type_support_handle<interface::srv::MoveJoints_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "interface/srv/detail/move_joints__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace interface
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoveJoints_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoveJoints_type_support_ids_t;

static const _MoveJoints_type_support_ids_t _MoveJoints_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoveJoints_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoveJoints_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoveJoints_type_support_symbol_names_t _MoveJoints_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, interface, srv, MoveJoints)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, interface, srv, MoveJoints)),
  }
};

typedef struct _MoveJoints_type_support_data_t
{
  void * data[2];
} _MoveJoints_type_support_data_t;

static _MoveJoints_type_support_data_t _MoveJoints_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoveJoints_service_typesupport_map = {
  2,
  "interface",
  &_MoveJoints_service_typesupport_ids.typesupport_identifier[0],
  &_MoveJoints_service_typesupport_symbol_names.symbol_name[0],
  &_MoveJoints_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t MoveJoints_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoveJoints_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<interface::srv::MoveJoints_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<interface::srv::MoveJoints_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<interface::srv::MoveJoints_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<interface::srv::MoveJoints>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<interface::srv::MoveJoints>,
  &interface__srv__MoveJoints__get_type_hash,
  &interface__srv__MoveJoints__get_type_description,
  &interface__srv__MoveJoints__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace interface

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<interface::srv::MoveJoints>()
{
  return &::interface::srv::rosidl_typesupport_cpp::MoveJoints_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, interface, srv, MoveJoints)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<interface::srv::MoveJoints>();
}

#ifdef __cplusplus
}
#endif
