#include <algorithm>
#include <memory>
// #include <unordered_map>
#include "commandqueue.h"
#include "types.h"
#include "nanextension.h"
#include "nan.h"

namespace opencl {

#define GET_WAIT_LIST(n)                                  \
  std::vector<NoCLEvent*> cl_events;                      \
  if (ARG_EXISTS(n)) {                                    \
    Local<Array> js_events = Local<Array>::Cast(info[n]); \
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);       \
  }

#define GET_EVENT_FLAG(n)                                 \
  cl_event event = nullptr;                               \
  cl_event* eventPtr = &event;                            \

#define GET_WAIT_LIST_AND_EVENT(n)                        \
  GET_WAIT_LIST(n)                                        \
  GET_EVENT_FLAG()

#define RETURN_EVENT                                        \
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event)); \

// /* Command Queue APIs */
// extern CL_API_ENTRY cl_command_queue CL_API_CALL
// clCreateCommandQueue(cl_context                     /* context */,
//                      cl_device_id                   /* device */,
//                      cl_command_queue_properties    /* properties */,
//                      cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateCommandQueue) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 1
  NOCL_UNWRAP(device, NoCLDeviceId, info[1]);

  cl_command_queue_properties properties = 0;
  
  if (ARG_EXISTS(2)) {
    properties = Nan::To<uint32_t>(info[2]).FromJust();
  }

  cl_int err;
  cl_command_queue q = ::clCreateCommandQueue(
    context->getRaw(), device->getRaw(), properties, &err);

  CHECK_ERR(err)
  info.GetReturnValue().Set(NOCL_WRAP(NoCLCommandQueue, q));
}

#ifdef CL_VERSION_2_0
// extern CL_API_ENTRY cl_command_queue CL_API_CALL
// clCreateCommandQueueWithProperties(cl_context               /* context */,
//                                    cl_device_id              /* device */,
//                                    const cl_queue_properties *    /* properties */,
//                                    cl_int *                 /* errcode_ret */) CL_API_SUFFIX__VERSION_2_0;
NAN_METHOD(CreateCommandQueueWithProperties) {
  Nan::HandleScope scope;
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 1
  NOCL_UNWRAP(device, NoCLDeviceId, info[1]);

  // Arg 2
  Local<Array> properties = Local<Array>::Cast(info[2]);
  vector<cl_queue_properties> cl_properties;

  for (uint32_t i=0; i < properties->Length(); i+=2) {
    if (!Nan::Get(properties, i).ToLocalChecked()->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
    }
    cl_uint prop_id = Nan::To<uint32_t>(Nan::Get(properties, i).ToLocalChecked()).FromJust();
    cl_properties.push_back(prop_id);

    if(prop_id == CL_QUEUE_PROPERTIES) {
      if (!Nan::Get(properties, i+1).ToLocalChecked()->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_queue_properties props = Nan::To<int32_t>(Nan::Get(properties, i+1).ToLocalChecked()).FromJust();
      cl_properties.push_back(props);
    } else if (prop_id == CL_QUEUE_SIZE) {
      if (!Nan::Get(properties, i+1).ToLocalChecked()->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_queue_properties size = Nan::To<int32_t>(Nan::Get(properties, i+1).ToLocalChecked()).FromJust();
      cl_properties.push_back(size);
    } else {
      THROW_ERR(CL_INVALID_QUEUE_PROPERTIES)
    }
  }
  cl_properties.push_back(0);

  cl_int err;
  cl_command_queue q = ::clCreateCommandQueueWithProperties(
    context->getRaw(),
    device->getRaw(),
    cl_properties.data(),
    &err);

  CHECK_ERR(err)

  info.GetReturnValue().Set(NOCL_WRAP(NoCLCommandQueue, q));
}
#endif

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainCommandQueue(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainCommandQueue) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  cl_int err=q->acquire();
  CHECK_ERR(err)

  info.GetReturnValue().Set(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseCommandQueue(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseCommandQueue) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  cl_int err=q->release();
  CHECK_ERR(err)
  info.GetReturnValue().Set(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetCommandQueueInfo(cl_command_queue      /* command_queue */,
//                       cl_command_queue_info /* param_name */,
//                       size_t                /* param_value_size */,
//                       void *                /* param_value */,
//                       size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetCommandQueueInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  cl_command_queue_info param_name = Nan::To<uint32_t>(info[1]).FromJust();

  switch(param_name) {
    case CL_QUEUE_CONTEXT: {
      cl_context val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_context), &val, nullptr))
      CHECK_ERR(::clRetainContext(val))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLContext, val));
      return;
    }
    case CL_QUEUE_DEVICE: {
      cl_device_id val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_device_id), &val, nullptr))
      CHECK_ERR(::clRetainDevice(val))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLDeviceId, val));
      return;
    }
    case CL_QUEUE_REFERENCE_COUNT: {
      cl_uint val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_uint), &val, nullptr))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_QUEUE_PROPERTIES: {
      cl_command_queue_properties val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_command_queue_properties), &val, nullptr))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
  }
  return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(CL_INVALID_VALUE)));
}

// /* Flush and Finish APIs */
// extern CL_API_ENTRY cl_int CL_API_CALL
// clFlush(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(Flush) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  cl_int err = ::clFlush(q->getRaw());

  CHECK_ERR(err);
  info.GetReturnValue().Set(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clFinish(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(Finish) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  cl_int err = ::clFinish(q->getRaw());

  CHECK_ERR(err);
  info.GetReturnValue().Set(JS_INT(err));
}

// /* Enqueued Commands APIs */
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueReadBuffer(cl_command_queue    /* command_queue */,
//                     cl_mem              /* buffer */,
//                     cl_bool             /* blocking_read */,
//                     size_t              /* offset */,
//                     size_t              /* size */,
//                     void *              /* ptr */,
//                     cl_uint             /* num_events_in_wait_list */,
//                     const cl_event *    /* event_wait_list */,
//                     cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueReadBuffer) {
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, info[1]);

  cl_bool blocking_read = Nan::To<bool>(info[2]).FromJust();
  size_t offset = Nan::To<uint32_t>(info[3]).FromJust();
  size_t size = Nan::To<uint32_t>(info[4]).FromJust();

  void *ptr=nullptr;
  if(info[5]->IsUndefined() || info[5]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else {
    size_t len=0;
    getPtrAndLen(info[5],ptr,len);
    // std::cout<<"[EnqueueReadBuffer] ptr 0x"<<std::hex<<ptr<<std::dec<<std::endl;

    if(!ptr || !len)
      return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  GET_WAIT_LIST_AND_EVENT(6)

  CHECK_ERR(::clEnqueueReadBuffer(
    q->getRaw(),buffer->getRaw(),blocking_read,offset,size,ptr,
    (cl_uint) cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueReadBufferRect(cl_command_queue    /* command_queue */,
//                         cl_mem              /* buffer */,
//                         cl_bool             /* blocking_read */,
//                         const size_t *      /* buffer_offset */,
//                         const size_t *      /* host_offset */,
//                         const size_t *      /* region */,
//                         size_t              /* buffer_row_pitch */,
//                         size_t              /* buffer_slice_pitch */,
//                         size_t              /* host_row_pitch */,
//                         size_t              /* host_slice_pitch */,
//                         void *              /* ptr */,
//                         cl_uint             /* num_events_in_wait_list */,
//                         const cl_event *    /* event_wait_list */,
//                         cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_1;
NAN_METHOD(EnqueueReadBufferRect) {
  Nan::HandleScope scope;
  REQ_ARGS(11);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, info[1]);

  // Arg 2
  cl_bool blocking_read = Nan::To<bool>(info[2]).FromJust();

  size_t buffer_offset[]={0,0,0};
  size_t host_offset[]={0,0,0};
  size_t region[]={1,1,1};

  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),3u);i++)
      buffer_offset[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),3u);i++)
      host_offset[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),3u);i++)
      region[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();

  size_t buffer_row_pitch = Nan::To<uint32_t>(info[6]).FromJust();
  size_t buffer_slice_pitch = Nan::To<uint32_t>(info[7]).FromJust();
  size_t host_row_pitch = Nan::To<uint32_t>(info[8]).FromJust();
  size_t host_slice_pitch = Nan::To<uint32_t>(info[9]).FromJust();

  void *ptr=nullptr;
  size_t len=0;

  if(info[10]->IsUndefined() || info[10]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
}
  else
    getPtrAndLen(info[10],ptr,len);
  if(!ptr || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  GET_WAIT_LIST_AND_EVENT(11)

  CHECK_ERR(::clEnqueueReadBufferRect(
    q->getRaw(),buffer->getRaw(),blocking_read,buffer_offset,host_offset,region,
    buffer_row_pitch,buffer_slice_pitch,host_row_pitch,host_slice_pitch,ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueWriteBuffer(cl_command_queue   /* command_queue */,
//                      cl_mem             /* buffer */,
//                      cl_bool            /* blocking_write */,
//                      size_t             /* offset */,
//                      size_t             /* size */,
//                      const void *       /* ptr */,
//                      cl_uint            /* num_events_in_wait_list */,
//                      const cl_event *   /* event_wait_list */,
//                      cl_event *         /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueWriteBuffer) {
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, info[1]);

  cl_bool blocking_write = Nan::To<bool>(info[2]).FromJust();
  size_t offset = Nan::To<uint32_t>(info[3]).FromJust();
  size_t size = Nan::To<uint32_t>(info[4]).FromJust();

  void *ptr=nullptr;
  if(info[5]->IsUndefined() || info[5]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else {
    size_t len=0;
    getPtrAndLen(info[5],ptr,len);

    if(!ptr || !len)
      return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  GET_WAIT_LIST_AND_EVENT(6)

  CHECK_ERR(::clEnqueueWriteBuffer(
    q->getRaw(),buffer->getRaw(),blocking_write,offset,size,ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueWriteBufferRect(cl_command_queue    /* command_queue */,
//                          cl_mem              /* buffer */,
//                          cl_bool             /* blocking_write */,
//                          const size_t *      /* buffer_offset */,
//                          const size_t *      /* host_offset */,
//                          const size_t *      /* region */,
//                          size_t              /* buffer_row_pitch */,
//                          size_t              /* buffer_slice_pitch */,
//                          size_t              /* host_row_pitch */,
//                          size_t              /* host_slice_pitch */,
//                          const void *        /* ptr */,
//                          cl_uint             /* num_events_in_wait_list */,
//                          const cl_event *    /* event_wait_list */,
//                          cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_1;
NAN_METHOD(EnqueueWriteBufferRect) {
  Nan::HandleScope scope;
  REQ_ARGS(11);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, info[1]);

  // Arg 2
  cl_bool blocking_write = Nan::To<bool>(info[2]).FromJust();

  size_t buffer_offset[]={0,0,0};
  size_t host_offset[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      buffer_offset[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      host_offset[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();

  size_t buffer_row_pitch = Nan::To<uint32_t>(info[6]).FromJust();
  size_t buffer_slice_pitch = Nan::To<uint32_t>(info[7]).FromJust();
  size_t host_row_pitch = Nan::To<uint32_t>(info[8]).FromJust();
  size_t host_slice_pitch = Nan::To<uint32_t>(info[9]).FromJust();

  void *ptr=nullptr;
  size_t len=0;
  if(info[10]->IsUndefined() || info[10]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(info[10],ptr,len);
  if(!ptr || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  GET_WAIT_LIST_AND_EVENT(11)

  CHECK_ERR(::clEnqueueWriteBufferRect(
    q->getRaw(),buffer->getRaw(),blocking_write,buffer_offset,host_offset,region,
    buffer_row_pitch,buffer_slice_pitch,host_row_pitch,host_slice_pitch,ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

#ifdef CL_VERSION_1_2
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueFillBuffer(cl_command_queue   /* command_queue */,
//                     cl_mem             /* buffer */,
//                     const void *       /* pattern */,
//                     size_t             /* pattern_size */,
//                     size_t             /* offset */,
//                     size_t             /* size */,
//                     cl_uint            /* num_events_in_wait_list */,
//                     const cl_event *   /* event_wait_list */,
//                     cl_event *         /* event */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(EnqueueFillBuffer) {
  Nan::HandleScope scope;
  REQ_ARGS(5);

  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  NOCL_UNWRAP(buffer, NoCLMem, info[1]);

  void *pattern=nullptr;
  cl_double scalar_pattern_double;
  cl_int scalar_pattern_int;
  size_t len=0;
  if(info[2]->IsUndefined() || info[2]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else if (info[2]->IsInt32()) {
    scalar_pattern_int = Nan::To<int32_t>(info[2]).FromJust();
    pattern = &scalar_pattern_int;
    len = sizeof(cl_uint);
  }
  else if (info[2]->IsNumber()) {
    scalar_pattern_double = Nan::To<double>(info[2]).FromJust();
    pattern = &scalar_pattern_double;
    len = sizeof(cl_double);
  }
  else {
    getPtrAndLen(info[2], pattern, len);
  }
    if(!pattern || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  size_t offset=Nan::To<uint32_t>(info[3]).FromJust();
  size_t size=Nan::To<uint32_t>(info[4]).FromJust();

  GET_WAIT_LIST_AND_EVENT(5)

  CHECK_ERR(::clEnqueueFillBuffer(
    q->getRaw(), buffer->getRaw(), pattern, len, offset, size,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}
#endif
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueCopyBuffer(cl_command_queue    /* command_queue */,
//                     cl_mem              /* src_buffer */,
//                     cl_mem              /* dst_buffer */,
//                     size_t              /* src_offset */,
//                     size_t              /* dst_offset */,
//                     size_t              /* size */,
//                     cl_uint             /* num_events_in_wait_list */,
//                     const cl_event *    /* event_wait_list */,
//                     cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueCopyBuffer) {
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(src_buffer, NoCLMem, info[1]);

  // Arg 2
  NOCL_UNWRAP(dst_buffer, NoCLMem, info[2]);


  size_t src_offset=Nan::To<uint32_t>(info[3]).FromJust();
  size_t dst_offset=Nan::To<uint32_t>(info[4]).FromJust();
  size_t size=Nan::To<uint32_t>(info[5]).FromJust();

  GET_WAIT_LIST_AND_EVENT(6)

  CHECK_ERR(::clEnqueueCopyBuffer(q->getRaw(),
    src_buffer->getRaw(),dst_buffer->getRaw(),src_offset,dst_offset, size,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueCopyBufferRect(cl_command_queue    /* command_queue */,
//                         cl_mem              /* src_buffer */,
//                         cl_mem              /* dst_buffer */,
//                         const size_t *      /* src_origin */,
//                         const size_t *      /* dst_origin */,
//                         const size_t *      /* region */,
//                         size_t              /* src_row_pitch */,
//                         size_t              /* src_slice_pitch */,
//                         size_t              /* dst_row_pitch */,
//                         size_t              /* dst_slice_pitch */,
//                         cl_uint             /* num_events_in_wait_list */,
//                         const cl_event *    /* event_wait_list */,
//                         cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_1;
NAN_METHOD(EnqueueCopyBufferRect) {
  Nan::HandleScope scope;
  REQ_ARGS(10);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(src_buffer, NoCLMem, info[1]);

  // Arg 2
  NOCL_UNWRAP(dst_buffer, NoCLMem, info[2]);

  size_t src_origin[]={0,0,0};
  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      src_origin[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      dst_origin[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  size_t src_row_pitch = Nan::To<uint32_t>(info[6]).FromJust();
  size_t src_slice_pitch = Nan::To<uint32_t>(info[7]).FromJust();
  size_t dst_row_pitch = Nan::To<uint32_t>(info[8]).FromJust();
  size_t dst_slice_pitch = Nan::To<uint32_t>(info[9]).FromJust();

  GET_WAIT_LIST_AND_EVENT(10)

  CHECK_ERR(::clEnqueueCopyBufferRect(
    q->getRaw(),src_buffer->getRaw(),dst_buffer->getRaw(),
    src_origin, dst_origin, region,
    src_row_pitch, src_slice_pitch, dst_row_pitch, dst_slice_pitch,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueReadImage(cl_command_queue     /* command_queue */,
//                    cl_mem               /* image */,
//                    cl_bool              /* blocking_read */,
//                    const size_t *       /* origin[3] */,
//                    const size_t *       /* region[3] */,
//                    size_t               /* row_pitch */,
//                    size_t               /* slice_pitch */,
//                    void *               /* ptr */,
//                    cl_uint              /* num_events_in_wait_list */,
//                    const cl_event *     /* event_wait_list */,
//                    cl_event *           /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueReadImage) {
  Nan::HandleScope scope;
  REQ_ARGS(8);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(image, NoCLMem, info[1]);

  cl_bool blocking_read = Nan::To<bool>(info[2]).FromJust();

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      origin[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();

  size_t row_pitch = Nan::To<uint32_t>(info[5]).FromJust();
  size_t slice_pitch = Nan::To<uint32_t>(info[6]).FromJust();

  void *ptr=nullptr;
  size_t len=0;
  if(info[7]->IsUndefined() || info[7]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else {
    getPtrAndLen(info[7], ptr, len);
  }
  if(!ptr || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  GET_WAIT_LIST_AND_EVENT(8)

  CHECK_ERR(::clEnqueueReadImage(q->getRaw(),image->getRaw(),blocking_read,
    origin,region,row_pitch,slice_pitch, ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueWriteImage(cl_command_queue    /* command_queue */,
//                     cl_mem              /* image */,
//                     cl_bool             /* blocking_write */,
//                     const size_t *      /* origin[3] */,
//                     const size_t *      /* region[3] */,
//                     size_t              /* input_row_pitch */,
//                     size_t              /* input_slice_pitch */,
//                     const void *        /* ptr */,
//                     cl_uint             /* num_events_in_wait_list */,
//                     const cl_event *    /* event_wait_list */,
//                     cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueWriteImage) {
  Nan::HandleScope scope;
  REQ_ARGS(8);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(image, NoCLMem, info[1]);

  // Arg 2
  cl_bool blocking_write = Nan::To<bool>(info[2]).FromJust();

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      origin[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();

  size_t row_pitch = Nan::To<uint32_t>(info[5]).FromJust();
  size_t slice_pitch = Nan::To<uint32_t>(info[6]).FromJust();

  void *ptr=nullptr;
  size_t len=0;
  if(info[7]->IsUndefined() || info[7]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(info[7],ptr,len);
  if(!ptr || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  GET_WAIT_LIST_AND_EVENT(8)

  CHECK_ERR(::clEnqueueWriteImage(q->getRaw(),image->getRaw(),blocking_write,
    origin,region,row_pitch,slice_pitch, ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

#ifdef CL_VERSION_1_2
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueFillImage(cl_command_queue   /* command_queue */,
//                    cl_mem             /* image */,
//                    const void *       /* fill_color */,
//                    const size_t *     /* origin[3] */,
//                    const size_t *     /* region[3] */,
//                    cl_uint            /* num_events_in_wait_list */,
//                    const cl_event *   /* event_wait_list */,
//                    cl_event *         /* event */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(EnqueueFillImage) {
  Nan::HandleScope scope;
  REQ_ARGS(5);

  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  NOCL_UNWRAP(image, NoCLMem, info[1]);

  void *fill_color=nullptr;
  size_t len=0;
  if(info[2]->IsUndefined() || info[2]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(info[2],fill_color,len);
  if(!fill_color || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      origin[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();

  GET_WAIT_LIST_AND_EVENT(5)

  CHECK_ERR(::clEnqueueFillImage(
    q->getRaw(),image->getRaw(),fill_color,
    origin,region,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}
#endif
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueCopyImage(cl_command_queue     /* command_queue */,
//                    cl_mem               /* src_image */,
//                    cl_mem               /* dst_image */,
//                    const size_t *       /* src_origin[3] */,
//                    const size_t *       /* dst_origin[3] */,
//                    const size_t *       /* region[3] */,
//                    cl_uint              /* num_events_in_wait_list */,
//                    const cl_event *     /* event_wait_list */,
//                    cl_event *           /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueCopyImage) {
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(src_image, NoCLMem, info[1]);

  // Arg 2
  NOCL_UNWRAP(dst_image, NoCLMem, info[2]);

  size_t src_origin[]={0,0,0};
  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      src_origin[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      dst_origin[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();

  GET_WAIT_LIST_AND_EVENT(6)

  CHECK_ERR(::clEnqueueCopyImage(
    q->getRaw(),src_image->getRaw(),dst_image->getRaw(),
    src_origin,dst_origin, region,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueCopyImageToBuffer(cl_command_queue /* command_queue */,
//                            cl_mem           /* src_image */,
//                            cl_mem           /* dst_buffer */,
//                            const size_t *   /* src_origin[3] */,
//                            const size_t *   /* region[3] */,
//                            size_t           /* dst_offset */,
//                            cl_uint          /* num_events_in_wait_list */,
//                            const cl_event * /* event_wait_list */,
//                            cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueCopyImageToBuffer) {
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(src_image, NoCLMem, info[1]);

  // Arg 2
  NOCL_UNWRAP(dst_buffer, NoCLMem, info[2]);

  size_t src_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      src_origin[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();

  size_t dst_offset = Nan::To<uint32_t>(info[5]).FromJust();

  GET_WAIT_LIST_AND_EVENT(6)

  CHECK_ERR(::clEnqueueCopyImageToBuffer(
    q->getRaw(),src_image->getRaw(),dst_buffer->getRaw(),
    src_origin, region, dst_offset,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueCopyBufferToImage(cl_command_queue /* command_queue */,
//                            cl_mem           /* src_buffer */,
//                            cl_mem           /* dst_image */,
//                            size_t           /* src_offset */,
//                            const size_t *   /* dst_origin[3] */,
//                            const size_t *   /* region[3] */,
//                            cl_uint          /* num_events_in_wait_list */,
//                            const cl_event * /* event_wait_list */,
//                            cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueCopyBufferToImage) {
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(src_buffer, NoCLMem, info[1]);

  // Arg 2
  NOCL_UNWRAP(dst_image, NoCLMem, info[2]);

  size_t src_offset = Nan::To<uint32_t>(info[3]).FromJust();

  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[4]);
  uint32_t i;

  for(i=0;i<max(arr->Length(),2u);i++)
      dst_origin[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=Nan::To<uint32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();

  GET_WAIT_LIST_AND_EVENT(6)

  CHECK_ERR(::clEnqueueCopyBufferToImage(
    q->getRaw(),src_buffer->getRaw(),dst_image->getRaw(),
    src_offset, dst_origin, region,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// static std::unordered_map<void*,int> mapPointers;

void CL_CALLBACK notifyMapCB (cl_event event, cl_int event_command_exec_status, void *user_data)
{
  // NoCLMapCB* asyncCB = static_cast<NoCLMapCB*>(user_data);
  // if(asyncCB!=nullptr)
  //     asyncCB->CallBackIsDone();
}

// extern CL_API_ENTRY void * CL_API_CALL
// clEnqueueMapBuffer(cl_command_queue /* command_queue */,
//                    cl_mem           /* buffer */,
//                    cl_bool          /* blocking_map */,
//                    cl_map_flags     /* map_flags */,
//                    size_t           /* offset */,
//                    size_t           /* size */,
//                    cl_uint          /* num_events_in_wait_list */,
//                    const cl_event * /* event_wait_list */,
//                    cl_event *       /* event */,
//                    cl_int *         /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueMapBuffer) {
  Nan::EscapableHandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(mem, NoCLMem, info[1]);

  cl_bool blocking_map = Nan::To<bool>(info[2]).FromJust() ? CL_TRUE : CL_FALSE;
  cl_map_flags map_flags = Nan::To<uint32_t>(info[3]).FromJust();
  size_t offset = Nan::To<uint32_t>(info[4]).FromJust();
  size_t size = Nan::To<uint32_t>(info[5]).FromJust();

  GET_WAIT_LIST_AND_EVENT(6)

  void* mPtr = nullptr;
  cl_int err;

  mPtr = clEnqueueMapBuffer(cq->getRaw(),mem->getRaw(),
                            blocking_map,map_flags, offset,
                            size, (cl_uint)cl_events.size(),
                            NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
                            eventPtr,&err);

  CHECK_ERR(err)

  Local<v8::ArrayBuffer> obj = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), mPtr, size);

  Nan::Set(obj, JS_STR("event"), NOCL_WRAP(NoCLEvent,event));

  if(!blocking_map) {
    // TODO? buf->SetIndexedPropertiesToExternalArrayData(NULL, buf->GetIndexedPropertiesExternalArrayDataType(), 0);
    // NoCLMapCB* cb = new NoCLMapCB(buf,size,mPtr);
    // err = clSetEventCallback(event,CL_COMPLETE,notifyMapCB,cb);
    // CHECK_ERR(err)
  }
  info.GetReturnValue().Set(scope.Escape(obj));
}

// extern CL_API_ENTRY void * CL_API_CALL
// clEnqueueMapImage(cl_command_queue  /* command_queue */,
//                   cl_mem            /* image */,
//                   cl_bool           /* blocking_map */,
//                   cl_map_flags      /* map_flags */,
//                   const size_t *    /* origin[3] */,
//                   const size_t *    /* region[3] */,
//                   size_t *          /* image_row_pitch */,
//                   size_t *          /* image_slice_pitch */,
//                   cl_uint           /* num_events_in_wait_list */,
//                   const cl_event *  /* event_wait_list */,
//                   cl_event *        /* event */,
//                   cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueMapImage) {
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(mem, NoCLMem, info[1]);

  cl_bool blocking_map = Nan::To<bool>(info[2]).FromJust();
  cl_map_flags map_flags = Nan::To<uint32_t>(info[3]).FromJust();
  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[4]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
    origin[i]=Nan::To<int32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
    region[i]=Nan::To<int32_t>(Nan::Get(arr, i).ToLocalChecked()).FromJust();

  size_t image_row_pitch;
  size_t image_slice_pitch;

  GET_WAIT_LIST_AND_EVENT(6)

  void* mPtr = nullptr;
  cl_int err;

  mPtr = clEnqueueMapImage (cq->getRaw(),mem->getRaw(),
                              blocking_map,map_flags, origin,
                              region,
                              &image_row_pitch, &image_slice_pitch,
                              (cl_uint)cl_events.size(),
                              NOCL_TO_CL_ARRAY(
                                cl_events, NoCLEvent),
                                eventPtr,&err);

  CHECK_ERR(err)

  size_t size = image_row_pitch * region[1];
  if (image_slice_pitch) {
    size = image_slice_pitch * region[2];
  }

  Local<v8::ArrayBuffer> obj = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), mPtr, size);

  Nan::Set(obj, JS_STR("event"), NOCL_WRAP(NoCLEvent,event));
  Nan::Set(obj, JS_STR("image_row_pitch"), Nan::New(static_cast<int>(image_row_pitch)));
  Nan::Set(obj, JS_STR("image_slice_pitch"), Nan::New(static_cast<int>(image_slice_pitch)));

  if(!blocking_map) {
    //TODO? buf->SetIndexedPropertiesToExternalArrayData(NULL, buf->GetIndexedPropertiesExternalArrayDataType(), 0);
    // NoCLMapCB* cb = new NoCLMapCB(buf,size,mPtr);
    // err = clSetEventCallback(event,CL_COMPLETE,notifyMapCB,cb);
    // CHECK_ERR(err)
  }
  info.GetReturnValue().Set(obj);
//
//  cl_event event=nullptr;
//  if(ARG_EXISTS(9)) {
//    NOCL_UNWRAP(evt, NoCLEvent, info[9]);
//    event = evt->getRaw();
//  }
//
//  cl_int ret=CL_SUCCESS;
//  void *ptr = ::clEnqueueMapImage(
//    q->getRaw(),image->getRaw(),blocking_map,map_flags,
//    origin, region, (size_t*)image_row_pitch, (size_t*)image_slice_pitch,
//    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
//    event ? &event : nullptr,
//    &ret);
//
//  CHECK_ERR(ret);
//
//  info.GetReturnValue().Set(NOCL_WRAP(NoCLMappedPtr, ptr));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueUnmapMemObject(cl_command_queue /* command_queue */,
//                         cl_mem           /* memobj */,
//                         void *           /* mapped_ptr */,
//                         cl_uint          /* num_events_in_wait_list */,
//                         const cl_event *  /* event_wait_list */,
//                         cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueUnmapMemObject) {
  Nan::HandleScope scope;
  REQ_ARGS(3);
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);
  NOCL_UNWRAP(mem, NoCLMem, info[1]);

  void *ptr=nullptr;
  size_t len=0;
  if(info[2]->IsUndefined() || info[2]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else {
    getPtrAndLen(info[2], ptr, len);
  }
  if(!ptr || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  GET_WAIT_LIST_AND_EVENT(3)

  cl_int err;
  err = clEnqueueUnmapMemObject(q->getRaw(),mem->getRaw(),ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent), eventPtr);
  CHECK_ERR(err)

  RETURN_EVENT
 }

#ifdef CL_VERSION_1_2
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueMigrateMemObjects(cl_command_queue       /* command_queue */,
//                            cl_uint                /* num_mem_objects */,
//                            const cl_mem *         /* mem_objects */,
//                            cl_mem_migration_flags /* flags */,
//                            cl_uint                /* num_events_in_wait_list */,
//                            const cl_event *       /* event_wait_list */,
//                            cl_event *             /* event */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(EnqueueMigrateMemObjects) {
  Nan::HandleScope scope;
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  if(info[1]->IsNull() || info[1]->IsUndefined() || !info[1]->IsArray()) {
    THROW_ERR(CL_INVALID_VALUE);
  }

  Local<Array> arr=Local<Array>::Cast(info[1]);
  uint32_t num_mem_objects=arr->Length();
  unique_ptr<cl_mem[]> mem_objects(new cl_mem[num_mem_objects]);
  for(uint32_t i=0;i<num_mem_objects;i++) {
    Local<Value> mem=Nan::Get(arr, i).ToLocalChecked();
    if (mem->IsNull() || mem->IsUndefined())
      THROW_ERR(CL_INVALID_MEM_OBJECT);
    NOCL_UNWRAP(obj, NoCLMem, mem);
    mem_objects[i]=obj->getRaw();
  }

  cl_mem_migration_flags flags=Nan::To<uint32_t>(info[2]).FromJust();

  GET_WAIT_LIST_AND_EVENT(3)

  CHECK_ERR(::clEnqueueMigrateMemObjects(
    q->getRaw(),num_mem_objects,
    mem_objects.get(),flags,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}
#endif
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueNDRangeKernel(cl_command_queue /* command_queue */,
//                        cl_kernel        /* kernel */,
//                        cl_uint          /* work_dim */,
//                        const size_t *   /* global_work_offset */,
//                        const size_t *   /* global_work_size */,
//                        const size_t *   /* local_work_size */,
//                        cl_uint          /* num_events_in_wait_list */,
//                        const cl_event * /* event_wait_list */,
//                        cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueNDRangeKernel) {
  Nan::HandleScope scope;
  REQ_ARGS(5);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(k, NoCLKernel, info[1]);


  cl_uint work_dim=Nan::To<uint32_t>(info[2]).FromJust();

  std::vector<size_t> cl_work_offset;
  std::vector<size_t> cl_work_global;
  std::vector<size_t> cl_work_local;

  if (ARG_EXISTS(3)) {
    Local<Array> js_work_offset = Local<Array>::Cast(info[3]);

    if (js_work_offset->Length() != work_dim) {
      THROW_ERR(CL_INVALID_GLOBAL_OFFSET);
    }

    for (unsigned int i = 0; i < work_dim; ++ i) {
      cl_work_offset.push_back(Nan::To<uint32_t>(Nan::Get(js_work_offset, i).ToLocalChecked()).FromJust());
    }
  }

  if (ARG_EXISTS(4)) {
    Local<Array> js_work_global = Local<Array>::Cast(info[4]);

    if (js_work_global->Length() != work_dim) {
      THROW_ERR(CL_INVALID_GLOBAL_WORK_SIZE);
    }

    for (unsigned int i = 0; i < work_dim; ++ i) {
      cl_work_global.push_back(Nan::To<uint32_t>(Nan::Get(js_work_global, i).ToLocalChecked()).FromJust());
    }
  }


  if (ARG_EXISTS(5)) {
    Local<Array> js_work_local = Local<Array>::Cast(info[5]);

    if (js_work_local->Length() != work_dim) {
      THROW_ERR(CL_INVALID_WORK_GROUP_SIZE);
    }

    for (unsigned int i = 0; i < work_dim; ++ i) {
      cl_work_local.push_back(Nan::To<uint32_t>(Nan::Get(js_work_local, i).ToLocalChecked()).FromJust());
    }
  }

  GET_WAIT_LIST_AND_EVENT(6)

  CHECK_ERR(::clEnqueueNDRangeKernel(
    q->getRaw(),
    k->getRaw(),
    work_dim,
    cl_work_offset.size() ? cl_work_offset.data() : nullptr,
    cl_work_global.size() ? cl_work_global.data() : nullptr,
    cl_work_local.size() ? cl_work_local.data() : nullptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr
  ));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueTask(cl_command_queue  /* command_queue */,
//               cl_kernel         /* kernel */,
//               cl_uint           /* num_events_in_wait_list */,
//               const cl_event *  /* event_wait_list */,
//               cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueTask) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(k, NoCLKernel, info[1]);


  GET_WAIT_LIST_AND_EVENT(2)

  CHECK_ERR(::clEnqueueTask(
    q->getRaw(),k->getRaw(),
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueNativeKernel(cl_command_queue  /* command_queue */,
//             void (CL_CALLBACK * /*user_func*/)(void *),
//                       void *            /* info */,
//                       size_t            /* cb_info */,
//                       cl_uint           /* num_mem_objects */,
//                       const cl_mem *    /* mem_list */,
//                       const void **     /* args_mem_loc */,
//                       cl_uint           /* num_events_in_wait_list */,
//                       const cl_event *  /* event_wait_list */,
//                       cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

// Note: only available if CL_EXEC_NATIVE_KERNEL capability
NAN_METHOD(EnqueueNativeKernel) {
  Nan::HandleScope scope;

  Nan::ThrowError("enqueueNativeKernel is not supported by Node OpenCL");

  return;
}


#ifdef CL_VERSION_1_2
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueMarkerWithWaitList(cl_command_queue /* command_queue */,
//                             cl_uint           /* num_events_in_wait_list */,
//                             const cl_event *  /* event_wait_list */,
//                             cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(EnqueueMarkerWithWaitList) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  GET_WAIT_LIST_AND_EVENT(1)

  CHECK_ERR(::clEnqueueMarkerWithWaitList(
    q->getRaw(),
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent), eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueBarrierWithWaitList(cl_command_queue /* command_queue */,
//                              cl_uint           /* num_events_in_wait_list */,
//                              const cl_event *  /* event_wait_list */,
//                              cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(EnqueueBarrierWithWaitList) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  GET_WAIT_LIST_AND_EVENT(1)

  CHECK_ERR(::clEnqueueBarrierWithWaitList(
    q->getRaw(),
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent), eventPtr));

  RETURN_EVENT
}
#endif

// // Deprecated OpenCL 1.1 APIs
#ifdef CL_VERSION_1_1
// extern CL_API_ENTRY CL_EXT_PREFIX__v11_DEPRECATED cl_int CL_API_CALL
// clEnqueueMarker(cl_command_queue    /* command_queue */,
//                 cl_event *          /* event */) CL_EXT_SUFFIX__v11_DEPRECATED;
NAN_METHOD(EnqueueMarker) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  GET_EVENT_FLAG(1)

  CHECK_ERR(::clEnqueueMarker(q->getRaw(), eventPtr));

  RETURN_EVENT
}

// extern CL_API_ENTRY CL_EXT_PREFIX__v11_DEPRECATED cl_int CL_API_CALL
// clEnqueueWaitForEvents(cl_command_queue /* command_queue */,
//                         cl_uint          /* num_events */,
//                         const cl_event * /* event_list */) CL_EXT_SUFFIX__v11_DEPRECATED;
NAN_METHOD(EnqueueWaitForEvents) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  GET_WAIT_LIST(1)

  CHECK_ERR(::clEnqueueWaitForEvents(q->getRaw(),
        cl_events.size(),cl_events.size() ? NOCL_TO_CL_ARRAY(cl_events, NoCLEvent): nullptr));


  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clEnqueueBarrier(cl_command_queue /* command_queue */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
NAN_METHOD(EnqueueBarrier) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  CHECK_ERR(::clEnqueueBarrier(q->getRaw()));

  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}
#endif

#ifdef CL_VERSION_2_0
// moved to svm.cpp

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueSVMFree(cl_command_queue  /* command_queue */,
//                  cl_uint           /* num_svm_pointers */,
//                  void *[]          /* svm_pointers[] */,
//                  void (CL_CALLBACK * /*pfn_free_func*/)(cl_command_queue /* queue */,
//                                                         cl_uint          /* num_svm_pointers */,
//                                                         void *[]         /* svm_pointers[] */,
//                                                         void *           /* user_data */),
//                  void *            /* user_data */,
//                  cl_uint           /* num_events_in_wait_list */,
//                  const cl_event *  /* event_wait_list */,
//                  cl_event *        /* event */) CL_API_SUFFIX__VERSION_2_0;

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueSVMMemcpy(cl_command_queue  /* command_queue */,
//                    cl_bool           /* blocking_copy */,
//                    void *            /* dst_ptr */,
//                    const void *      /* src_ptr */,
//                    size_t            /* size */,
//                    cl_uint           /* num_events_in_wait_list */,
//                    const cl_event *  /* event_wait_list */,
//                    cl_event *        /* event */) CL_API_SUFFIX__VERSION_2_0;

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueSVMMemFill(cl_command_queue  /* command_queue */,
//                     void *            /* svm_ptr */,
//                     const void *      /* pattern */,
//                     size_t            /* pattern_size */,
//                     size_t            /* size */,
//                     cl_uint           /* num_events_in_wait_list */,
//                     const cl_event *  /* event_wait_list */,
//                     cl_event *        /* event */) CL_API_SUFFIX__VERSION_2_0;

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueSVMMap(cl_command_queue  /* command_queue */,
//                 cl_bool           /* blocking_map */,
//                 cl_map_flags      /* flags */,
//                 void *            /* svm_ptr */,
//                 size_t            /* size */,
//                 cl_uint           /* num_events_in_wait_list */,
//                 const cl_event *  /* event_wait_list */,
//                 cl_event *        /* event */) CL_API_SUFFIX__VERSION_2_0;

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueSVMUnmap(cl_command_queue  /* command_queue */,
//                   void *            /* svm_ptr */,
//                   cl_uint           /* num_events_in_wait_list */,
//                   const cl_event *  /* event_wait_list */,
//                   cl_event *        /* event */) CL_API_SUFFIX__VERSION_2_0;
#endif

#ifdef CL_VERSION_2_1
// moved to svm.cpp
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueSVMMigrateMem(cl_command_queue         /* command_queue */,
//                        cl_uint                  /* num_svm_pointers */,
//                        const void **            /* svm_pointers */,
//                        const size_t *           /* sizes */,
//                        cl_mem_migration_flags   /* flags */,
//                        cl_uint                  /* num_events_in_wait_list */,
//                        const cl_event *         /* event_wait_list */,
//                        cl_event *               /* event */) CL_API_SUFFIX__VERSION_2_1;
#endif

namespace CommandQueue {
NAN_MODULE_INIT(init)
{
  Nan::SetMethod(target, "createCommandQueue", CreateCommandQueue);
#ifdef CL_VERSION_2_0
  Nan::SetMethod(target, "createCommandQueueWithProperties", CreateCommandQueueWithProperties);
#endif
  Nan::SetMethod(target, "retainCommandQueue", RetainCommandQueue);
  Nan::SetMethod(target, "releaseCommandQueue", ReleaseCommandQueue);
  Nan::SetMethod(target, "getCommandQueueInfo", GetCommandQueueInfo);
  Nan::SetMethod(target, "flush", Flush);
  Nan::SetMethod(target, "finish", Finish);
  Nan::SetMethod(target, "enqueueReadBuffer", EnqueueReadBuffer);
  Nan::SetMethod(target, "enqueueReadBufferRect", EnqueueReadBufferRect);
  Nan::SetMethod(target, "enqueueWriteBuffer", EnqueueWriteBuffer);
  Nan::SetMethod(target, "enqueueWriteBufferRect", EnqueueWriteBufferRect);
  Nan::SetMethod(target, "enqueueCopyBuffer", EnqueueCopyBuffer);
  Nan::SetMethod(target, "enqueueCopyBufferRect", EnqueueCopyBufferRect);
  Nan::SetMethod(target, "enqueueReadImage", EnqueueReadImage);
  Nan::SetMethod(target, "enqueueWriteImage", EnqueueWriteImage);
  Nan::SetMethod(target, "enqueueCopyImage", EnqueueCopyImage);
  Nan::SetMethod(target, "enqueueCopyImageToBuffer", EnqueueCopyImageToBuffer);
  Nan::SetMethod(target, "enqueueCopyBufferToImage", EnqueueCopyBufferToImage);
  Nan::SetMethod(target, "enqueueMapBuffer", EnqueueMapBuffer);
  Nan::SetMethod(target, "enqueueMapImage", EnqueueMapImage);
  Nan::SetMethod(target, "enqueueUnmapMemObject", EnqueueUnmapMemObject);
  Nan::SetMethod(target, "enqueueNDRangeKernel", EnqueueNDRangeKernel);
  Nan::SetMethod(target, "enqueueTask", EnqueueTask); // removed in 2.0
  Nan::SetMethod(target, "enqueueNativeKernel", EnqueueNativeKernel);
#ifdef CL_VERSION_1_2
  Nan::SetMethod(target, "enqueueMarkerWithWaitList", EnqueueMarkerWithWaitList);
  Nan::SetMethod(target, "enqueueBarrierWithWaitList", EnqueueBarrierWithWaitList);
  Nan::SetMethod(target, "enqueueFillBuffer", EnqueueFillBuffer);
  Nan::SetMethod(target, "enqueueFillImage", EnqueueFillImage);
  Nan::SetMethod(target, "enqueueMigrateMemObjects", EnqueueMigrateMemObjects);
#endif
#ifdef CL_VERSION_1_1
  Nan::SetMethod(target, "enqueueWaitForEvents", EnqueueWaitForEvents);
  Nan::SetMethod(target, "enqueueMarker", EnqueueMarker);
  Nan::SetMethod(target, "enqueueBarrier", EnqueueBarrier);
#endif
}
} // namespace CommandQueue

} // namespace opencl
