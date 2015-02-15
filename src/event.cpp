#include <AVFoundation/AVFoundation.h>
#include "event.h"
#include "types.h"

namespace opencl {

// /* Event Object APIs */
// extern CL_API_ENTRY cl_int CL_API_CALL
// clWaitForEvents(cl_uint             /* num_events */,
//                 const cl_event *    /* event_list */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(WaitForEvents) {
  NanScope();
  REQ_ARGS(1);

  std::vector<NoCLEvent> events;
  Local<Array> js_events = Local<Array>::Cast(args[1]);
  NOCL_TO_ARRAY(events, js_events, NoCLEvent);

  CHECK_ERR(::clWaitForEvents(
    events.size(), TO_CL_ARRAY(events, NoCLEvent)));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetEventInfo(cl_event         /* event */,
//                cl_event_info    /* param_name */,
//                size_t           /* param_value_size */,
//                void *           /* param_value */,
//                size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetEventInfo) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(ev, NoCLEvent, args[0]);

  // Arg 1
  cl_event_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_EVENT_COMMAND_QUEUE:
    {
      cl_command_queue val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_command_queue), &val, NULL))
      // TODO NanReturnValue(JS_INT(val));
      break;
    }
    case CL_EVENT_CONTEXT:
    {
      cl_context val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_context), &val, NULL))
      // TODO NanReturnValue(JS_INT(val));
      break;
    }
    case CL_EVENT_COMMAND_TYPE:
    {
      cl_command_type val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_command_type), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_EVENT_COMMAND_EXECUTION_STATUS:
    {
      cl_int val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_int), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_EVENT_REFERENCE_COUNT:
    {
      cl_uint val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
  }
  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

// extern CL_API_ENTRY cl_event CL_API_CALL
// clCreateUserEvent(cl_context    /* context */,
//                   cl_int *      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;
NAN_METHOD(CreateUserEvent) {
  NanScope();
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  cl_int err;
  cl_event uev=::clCreateUserEvent(context->getRaw(), &err);
  CHECK_ERR(err)
  NanReturnValue(NOCL_WRAP(NoCLEvent, uev));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainEvent(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainEvent) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(ev, NoCLEvent, args[0]);
  cl_int count=clRetainEvent(ev->getRaw());

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseEvent(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseEvent) {
  NanScope();
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(ev, NoCLEvent, args[0]);
  cl_int count=clReleaseEvent(ev->getRaw());

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetUserEventStatus(cl_event   /* event */,
//                      cl_int     /* execution_status */) CL_API_SUFFIX__VERSION_1_1;
NAN_METHOD(SetUserEventStatus) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(ev, NoCLEvent, args[0]);

  cl_int exec_status=args[1]->Uint32Value();
  CHECK_ERR(::clSetUserEventStatus(ev->getRaw(),exec_status));

  NanReturnUndefined(); // TODO should we return err?
}

// /* Profiling APIs */
// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetEventProfilingInfo(cl_event            /* event */,
//                         cl_profiling_info   /* param_name */,
//                         size_t              /* param_value_size */,
//                         void *              /* param_value */,
//                         size_t *            /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetEventProfilingInfo) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(ev, NoCLEvent, args[0]);

  cl_profiling_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_PROFILING_COMMAND_QUEUED:
    case CL_PROFILING_COMMAND_SUBMIT:
    case CL_PROFILING_COMMAND_START:
    case CL_PROFILING_COMMAND_END:
    {
      cl_ulong val;
      CHECK_ERR(::clGetEventProfilingInfo(ev->getRaw(),param_name,sizeof(cl_ulong), &val, NULL))
      NanReturnValue(JS_INT((uint32_t) val)); // TODO how do we return 64b value in JS?
    }
  }
  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

namespace Event {
void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "waitForEvents", WaitForEvents);
  NODE_SET_METHOD(exports, "getEventInfo", GetEventInfo);
  NODE_SET_METHOD(exports, "createUserEvent", CreateUserEvent);
  NODE_SET_METHOD(exports, "retainEvent", RetainEvent);
  NODE_SET_METHOD(exports, "releaseEvent", ReleaseEvent);
  NODE_SET_METHOD(exports, "setUserEventStatus", SetUserEventStatus);
  NODE_SET_METHOD(exports, "getEventProfilingInfo", GetEventProfilingInfo);
}
} // namespace Event

} // namespace opencl
