#include "event.h"
#include "types.h"

namespace opencl {

// /* Event Object APIs */
// extern CL_API_ENTRY cl_int CL_API_CALL
// clWaitForEvents(cl_uint             /* num_events */,
//                 const cl_event *    /* event_list */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(WaitForEvents) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  std::vector<NoCLEvent> events;
  Local<Array> js_events = Local<Array>::Cast(info[0]);
  NOCL_TO_ARRAY(events, js_events, NoCLEvent);

  CHECK_ERR(::clWaitForEvents(
    (cl_uint) events.size(), NOCL_TO_CL_ARRAY(events, NoCLEvent)));

  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetEventInfo(cl_event         /* event */,
//                cl_event_info    /* param_name */,
//                size_t           /* param_value_size */,
//                void *           /* param_value */,
//                size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetEventInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(ev, NoCLEvent, info[0]);

  // Arg 1
  cl_event_info param_name = info[1]->Uint32Value();

  switch(param_name) {
    case CL_EVENT_COMMAND_QUEUE:
    {
      cl_command_queue val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_command_queue), &val, NULL))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLCommandQueue, val));
      return;
    }
    case CL_EVENT_CONTEXT:
    {
      cl_context val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_context), &val, NULL))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLContext, val));
      return;
    }
    case CL_EVENT_COMMAND_TYPE:
    {
      cl_command_type val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_command_type), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_EVENT_COMMAND_EXECUTION_STATUS:
    {
      cl_int val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_int), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_EVENT_REFERENCE_COUNT:
    {
      cl_uint val;
      CHECK_ERR(::clGetEventInfo(ev->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
  }
  return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(CL_INVALID_VALUE)));
}

// extern CL_API_ENTRY cl_event CL_API_CALL
// clCreateUserEvent(cl_context    /* context */,
//                   cl_int *      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;
NAN_METHOD(CreateUserEvent) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  cl_int err;
  cl_event uev=::clCreateUserEvent(context->getRaw(), &err);
  CHECK_ERR(err)
  info.GetReturnValue().Set(NOCL_WRAP_AND_RELEASE(NoCLEvent, uev));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainEvent(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainEvent) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(ev, NoCLEvent, info[0]);
  cl_int err=ev->acquire();
  //cl_int err=clRetainEvent(ev->getRaw());
  CHECK_ERR(err)
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseEvent(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseEvent) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(ev, NoCLEvent, info[0]);
  cl_int err=ev->release();
  //cl_int err=clReleaseEvent(ev->getRaw());

  CHECK_ERR(err)
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetUserEventStatus(cl_event   /* event */,
//                      cl_int     /* execution_status */) CL_API_SUFFIX__VERSION_1_1;
NAN_METHOD(SetUserEventStatus) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(ev, NoCLEvent, info[0]);

  cl_int exec_status=info[1]->Uint32Value();
  CHECK_ERR(::clSetUserEventStatus(ev->getRaw(),exec_status));

  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// /* Profiling APIs */
// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetEventProfilingInfo(cl_event            /* event */,
//                         cl_profiling_info   /* param_name */,
//                         size_t              /* param_value_size */,
//                         void *              /* param_value */,
//                         size_t *            /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetEventProfilingInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(ev, NoCLEvent, info[0]);

  cl_profiling_info param_name = info[1]->Uint32Value();

  switch(param_name) {
    case CL_PROFILING_COMMAND_QUEUED:
    case CL_PROFILING_COMMAND_SUBMIT:
    case CL_PROFILING_COMMAND_START:
    case CL_PROFILING_COMMAND_END:
    {
      /**
        JS Compatibility

        As JS does not support 64 bits integer, we return a 2-integer array with
          output_values[0] = (input_value >> 32) & 0xffffffff;
          output_values[1] = input_value & 0xffffffff;

        and reconstruction as
          input_value = ((int64_t) output_values[0]) << 32) | output_values[1];
      */
      cl_ulong val;
      CHECK_ERR(::clGetEventProfilingInfo(ev->getRaw(),param_name,sizeof(cl_ulong), &val, NULL))

      Local<Array> arr = Nan::New<Array>(2);
      arr->Set(0, JS_INT((uint32_t) (val>>32))); // hi
      arr->Set(1, JS_INT((uint32_t) (val & 0xffffffff))); // lo
      info.GetReturnValue().Set(arr);
      return;
    }
  }
  return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(CL_INVALID_VALUE)));
}

class NoCLEventWorker : public AsyncWorker
{
public:
  NoCLEventWorker(Callback* callback, const v8::Local<v8::Object> &userData,const v8::Local<v8::Object> &noCLEvent) :
  AsyncWorker(callback)
  {
    SaveToPersistent(kIndex,  userData);
    SaveToPersistent(kIndex+1,noCLEvent);
  }

  ~NoCLEventWorker() {}

  void CallBackIsDone(int status) {
    mCLCallbackStatus = status;
  }

  // Executed inside the worker-thread.
  // not safe to use V8 calls
  void Execute()
  {
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    Nan::EscapableHandleScope  scope;

    Local<Value> argv[] = {
      GetFromPersistent(kIndex),  // event
      JS_INT(mCLCallbackStatus),  // error status
      GetFromPersistent(kIndex+1) // userdata
    };
    callback->Call(3,argv);
  }

protected:
  static const uint32_t kIndex = 0;

 private:
   int mCLCallbackStatus = 0;
};

void CL_CALLBACK notifyCB (cl_event event, cl_int event_command_exec_status, void *user_data) {
  NoCLEventWorker* asyncCB = static_cast<NoCLEventWorker*>(user_data);
  asyncCB->CallBackIsDone(event_command_exec_status);
  AsyncQueueWorker(asyncCB);
}

NAN_METHOD(SetEventCallback)
{
  Nan::HandleScope scope;
  REQ_ARGS(3);
  NOCL_UNWRAP(event, NoCLEvent, info[0]);
  cl_int callbackStatusType = info[1]->Int32Value();
  Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());
  Local<Object> userData = info[3].As<Object>();

  NoCLEventWorker* asyncCB = new NoCLEventWorker(callback,userData,info[0].As<Object>());

  CHECK_ERR(clSetEventCallback(event->getRaw(),callbackStatusType,notifyCB,asyncCB));

  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}



namespace Event {
NAN_MODULE_INIT(init)
{
  Nan::SetMethod(target, "waitForEvents", WaitForEvents);
  Nan::SetMethod(target, "getEventInfo", GetEventInfo);
  Nan::SetMethod(target, "createUserEvent", CreateUserEvent);
  Nan::SetMethod(target, "retainEvent", RetainEvent);
  Nan::SetMethod(target, "releaseEvent", ReleaseEvent);
  Nan::SetMethod(target, "setUserEventStatus", SetUserEventStatus);
  Nan::SetMethod(target, "setEventCallback", SetEventCallback);
  Nan::SetMethod(target, "getEventProfilingInfo", GetEventProfilingInfo);
}
} // namespace Event

} // namespace opencl
