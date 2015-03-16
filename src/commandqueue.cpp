#include "commandqueue.h"
#include "types.h"
#include <map>
#include "nanextension.h"
#include <algorithm>

namespace opencl {

#ifndef CL_VERSION_2_0

// /* Command Queue APIs */
// extern CL_API_ENTRY cl_command_queue CL_API_CALL
// clCreateCommandQueue(cl_context                     /* context */,
//                      cl_device_id                   /* device */,
//                      cl_command_queue_properties    /* properties */,
//                      cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateCommandQueue) {
  NanScope();
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 1
  NOCL_UNWRAP(device, NoCLDeviceId, args[1]);

  cl_command_queue_properties properties = args[2]->Uint32Value();

  cl_int err;
  cl_command_queue q = ::clCreateCommandQueue(
    context->getRaw(), device->getRaw(), properties, &err);

  CHECK_ERR(err)
  NanReturnValue(NOCL_WRAP(NoCLCommandQueue, q));
}

#else

// extern CL_API_ENTRY cl_command_queue CL_API_CALL
// clCreateCommandQueueWithProperties(cl_context               /* context */,
//                                    cl_device_id              /* device */,
//                                    const cl_queue_properties *    /* properties */,
//                                    cl_int *                 /* errcode_ret */) CL_API_SUFFIX__VERSION_2_0;
NAN_METHOD(CreateCommandQueueWithProperties) {
  NanScope();
  REQ_ARGS(3);

  // Arg 1
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 2
  NOCL_UNWRAP(device, NoCLDeviceId, args[1]);

  // Arg 3
  Local<Array> properties = Local<Array>::Cast(args[2]);
  vector<cl_queue_properties> cl_properties;

  for (uint32_t i=0; i < properties->Length(); i+=2) {
    if (!properties->Get(i)->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
    }
    cl_uint prop_id = properties->Get(i)->Uint32Value();
    cl_properties.push_back(prop_id);

    if(prop_id == CL_QUEUE_PROPERTIES) {
      if (!properties->Get(i+1)->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_queue_properties props = properties->Get(i+1)->Int32Value();
      cl_properties.push_back(props);
    } else if (prop_id == CL_QUEUE_SIZE) {
      if (!properties->Get(i+1)->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_queue_properties size = properties->Get(i+1)->Int32Value();
      cl_properties.push_back(size);
    } else {
      THROW_ERR(CL_INVALID_QUEUE_PROPERTIES)
    }
  }

  cl_int err;
  cl_command_queue q = ::clCreateCommandQueueWithProperties(
    context->getRaw(),
    device->getRaw(),
    cl_properties.data(),
    &err);

  CHECK_ERR(err)

  NanReturnValue(NOCL_WRAP(NoCLCommandQueue, q));
}
#endif

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainCommandQueue(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainCommandQueue) {
  NanScope();
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  cl_int err=q->acquire();
  CHECK_ERR(err)

  NanReturnValue(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseCommandQueue(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseCommandQueue) {
  NanScope();
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  cl_int err=q->release();
  CHECK_ERR(err)
  NanReturnValue(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetCommandQueueInfo(cl_command_queue      /* command_queue */,
//                       cl_command_queue_info /* param_name */,
//                       size_t                /* param_value_size */,
//                       void *                /* param_value */,
//                       size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetCommandQueueInfo) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  cl_command_queue_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_QUEUE_CONTEXT: {
      cl_context val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_context), &val, nullptr))
      NanReturnValue(NOCL_WRAP(NoCLContext, val));
    }
    case CL_QUEUE_DEVICE: {
      cl_device_id val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_device_id), &val, nullptr))
      NanReturnValue(NOCL_WRAP(NoCLDeviceId, val));
    }
    case CL_QUEUE_REFERENCE_COUNT: {
      cl_uint val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_uint), &val, nullptr))
      NanReturnValue(JS_INT(val));
    }
    case CL_QUEUE_PROPERTIES: {
      cl_command_queue_properties val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_command_queue_properties), &val, nullptr))
      NanReturnValue(JS_INT(val));
    }
  }
  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

// /* Flush and Finish APIs */
// extern CL_API_ENTRY cl_int CL_API_CALL
// clFlush(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(Flush) {
  NanScope();
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  cl_int err = ::clFlush(q->getRaw());

  CHECK_ERR(err);
  NanReturnValue(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clFinish(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(Finish) {
  NanScope();
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  cl_int err = ::clFinish(q->getRaw());

  CHECK_ERR(err);
  NanReturnValue(JS_INT(err));
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
  NanScope();
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, args[1]);

  cl_bool blocking_read = args[2]->BooleanValue();
  size_t offset = args[3]->Uint32Value();
  size_t size = args[4]->Uint32Value();

  void *ptr=nullptr;
  int len=0;
  if(args[5]->IsUndefined() || args[5]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[5],ptr,len);

  std::vector<NoCLEvent> cl_events;
  
  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(args[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  //::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_context), &val, nullptr)
  cl_event event=nullptr;
  if(ARG_EXISTS(7) && args[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueReadBuffer(
    q->getRaw(),buffer->getRaw(),blocking_read,offset,size,ptr,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(11);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, args[1]);

  // Arg 2
  cl_bool blocking_read = args[2]->BooleanValue();

  size_t buffer_offset[]={0,0,0};
  size_t host_offset[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      buffer_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      host_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t buffer_row_pitch = args[6]->Uint32Value();
  size_t buffer_slice_pitch = args[7]->Uint32Value();
  size_t host_row_pitch = args[8]->Uint32Value();
  size_t host_slice_pitch = args[9]->Uint32Value();

  void *ptr=nullptr;
  int len=0;

  if(args[10]->IsUndefined() || args[10]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[10],ptr,len);

  std::vector<NoCLEvent> cl_events;

  if (ARG_EXISTS(11)) {
    Local<Array> js_events = Local<Array>::Cast(args[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(12) && args[12]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueReadBufferRect(
    q->getRaw(),buffer->getRaw(),blocking_read,buffer_offset,host_offset,region,
    buffer_row_pitch,buffer_slice_pitch,host_row_pitch,host_slice_pitch,ptr,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, args[1]);

  cl_bool blocking_write = args[2]->BooleanValue();
  size_t offset = args[3]->Uint32Value();
  size_t size = args[4]->Uint32Value();

  void *ptr=nullptr;
  int len=0;
  if(args[5]->IsUndefined() || args[5]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[5],ptr,len);

  std::vector<NoCLEvent> cl_events;

  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(args[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);

  }
  cl_event event=nullptr;
  if(ARG_EXISTS(7) && args[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueWriteBuffer(
    q->getRaw(),buffer->getRaw(),blocking_write,offset,size,ptr,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(11);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, args[1]);

  // Arg 2
  cl_bool blocking_write = args[2]->BooleanValue();

  size_t buffer_offset[]={0,0,0};
  size_t host_offset[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      buffer_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      host_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t buffer_row_pitch = args[6]->Uint32Value();
  size_t buffer_slice_pitch = args[7]->Uint32Value();
  size_t host_row_pitch = args[8]->Uint32Value();
  size_t host_slice_pitch = args[9]->Uint32Value();

  void *ptr=nullptr;
  int len=0;
  if(args[10]->IsUndefined() || args[10]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[10],ptr,len);

  std::vector<NoCLEvent> cl_events;
  if (ARG_EXISTS(12)) {
    Local<Array> js_events = Local<Array>::Cast(args[11]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }


  cl_event event=nullptr;
  if(ARG_EXISTS(12) && args[12]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueWriteBufferRect(
    q->getRaw(),buffer->getRaw(),blocking_write,buffer_offset,host_offset,region,
    buffer_row_pitch,buffer_slice_pitch,host_row_pitch,host_slice_pitch,ptr,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(5);

  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  NOCL_UNWRAP(buffer, NoCLMem, args[1]);

  void *pattern=nullptr;
  cl_double scalar_pattern_double;
  cl_int scalar_pattern_int;
  int len=0;
  if(args[2]->IsUndefined() || args[2]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else if (args[2]->IsInt32()) {
    scalar_pattern_int = args[2]->Int32Value();
    pattern = &scalar_pattern_int;
    len = sizeof(cl_uint);
  }
  else if (args[2]->IsNumber()) {
    scalar_pattern_double = args[2]->NumberValue();
    pattern = &scalar_pattern_double;
    len = sizeof(cl_double);
  }
  else {
    getPtrAndLen(args[2], pattern, len);
  }

  size_t offset=args[3]->Uint32Value();
  size_t size=args[4]->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if (ARG_EXISTS(5)) {
    Local<Array> js_events = Local<Array>::Cast(args[5]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(6) && args[6]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueFillBuffer(
    q->getRaw(), buffer->getRaw(), pattern, len, offset, size,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(src_buffer, NoCLMem, args[1]);

  // Arg 2
  NOCL_UNWRAP(dst_buffer, NoCLMem, args[2]);


  size_t src_offset=args[3]->Uint32Value();
  size_t dst_offset=args[4]->Uint32Value();
  size_t size=args[5]->Uint32Value();

  std::vector<NoCLEvent> cl_events;

  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(args[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && args[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueCopyBuffer(q->getRaw(),
    src_buffer->getRaw(),dst_buffer->getRaw(),src_offset,dst_offset, size,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent), &event));
  NanReturnValue(JS_INT(CL_SUCCESS));
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
  NanScope();
  REQ_ARGS(10);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(src_buffer, NoCLMem, args[1]);

  // Arg 2
  NOCL_UNWRAP(dst_buffer, NoCLMem, args[2]);

  size_t src_origin[]={0,0,0};
  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      src_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      dst_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();
  size_t src_row_pitch = args[6]->Uint32Value();
  size_t src_slice_pitch = args[7]->Uint32Value();
  size_t dst_row_pitch = args[8]->Uint32Value();
  size_t dst_slice_pitch = args[9]->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(10)) {
    Local<Array> js_events = Local<Array>::Cast(args[10]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(11) && args[11]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueCopyBufferRect(
    q->getRaw(),src_buffer->getRaw(),dst_buffer->getRaw(),
    src_origin, dst_origin, region,
    src_row_pitch, src_slice_pitch, dst_row_pitch, dst_slice_pitch,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(8);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(image, NoCLMem, args[1]);

  cl_bool blocking_read = args[2]->BooleanValue();

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t row_pitch = args[5]->Uint32Value();
  size_t slice_pitch = args[6]->Uint32Value();

  void *ptr=nullptr;
  int len=0;
  if(args[7]->IsUndefined() || args[7]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else {
    getPtrAndLen(args[7], ptr, len);
  }

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(8)) {
    Local<Array> js_events = Local<Array>::Cast(args[8]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(9) && args[9]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueReadImage(q->getRaw(),image->getRaw(),blocking_read,
    origin,region,row_pitch,slice_pitch, ptr,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(8);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(image, NoCLMem, args[1]);

  // Arg 2
  cl_bool blocking_write = args[2]->BooleanValue();

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t row_pitch = args[5]->Uint32Value();
  size_t slice_pitch = args[6]->Uint32Value();

  void *ptr=nullptr;
  int len=0;
  if(args[7]->IsUndefined() || args[7]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[7],ptr,len);

  std::vector<NoCLEvent> cl_events;
  if (ARG_EXISTS(8)) {
    Local<Array> js_events = Local<Array>::Cast(args[8]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(9) && args[9]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueWriteImage(q->getRaw(),image->getRaw(),blocking_write,
    origin,region,row_pitch,slice_pitch, ptr,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(5);

  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  NOCL_UNWRAP(image, NoCLMem, args[1]);

  void *fill_color=nullptr;
  int len=0;
  if(args[2]->IsUndefined() || args[2]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[2],fill_color,len);

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(5)) {
    Local<Array> js_events = Local<Array>::Cast(args[5]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(6) && args[6]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueFillImage(
    q->getRaw(),image->getRaw(),fill_color,
    origin,region,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(src_image, NoCLMem, args[1]);

  // Arg 2
  NOCL_UNWRAP(dst_image, NoCLMem, args[2]);

  size_t src_origin[]={0,0,0};
  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      src_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      dst_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(args[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && args[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueCopyImage(
    q->getRaw(),src_image->getRaw(),dst_image->getRaw(),
    src_origin,dst_origin, region,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(src_image, NoCLMem, args[1]);

  // Arg 2
  NOCL_UNWRAP(dst_buffer, NoCLMem, args[2]);

  size_t src_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      src_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t dst_offset = args[5]->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(args[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && args[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueCopyImageToBuffer(
    q->getRaw(),src_image->getRaw(),dst_buffer->getRaw(),
    src_origin, region, dst_offset,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(src_buffer, NoCLMem, args[1]);

  // Arg 2
  NOCL_UNWRAP(dst_image, NoCLMem, args[2]);

  size_t src_offset = args[3]->Uint32Value();

  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[4]);
  uint32_t i;

  for(i=0;i<max(arr->Length(),2u);i++)
      dst_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(args[7]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && args[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueCopyBufferToImage(
    q->getRaw(),src_buffer->getRaw(),dst_image->getRaw(),
    src_offset, dst_origin, region,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
}

static std::map<void*,int> mapPointers;

void CL_CALLBACK notifyMapCB (cl_event event, cl_int event_command_exec_status, void *user_data) {
    NoCLMapCB* asyncCB = static_cast<NoCLMapCB*>(user_data);
    if(asyncCB!=nullptr)
        asyncCB->CallBackIsDone();
}

struct unMapInfo {
  cl_command_queue cq;
  cl_mem mem;
};

void freeMapedPtr(char* ptr,void* hint) {
    mapPointers[ptr]--;
    unMapInfo* umi = static_cast<unMapInfo*>(hint);
    clEnqueueUnmapMemObject (umi->cq ,
        umi->mem,
        ptr,
        0 ,
        nullptr ,
        nullptr );
    clReleaseCommandQueue(umi->cq);
    clReleaseMemObject(umi->mem);
    delete umi;
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
  NanScope();
  REQ_ARGS(8);

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(mem, NoCLMem, args[1]);


  cl_bool blocking_map = args[2]->BooleanValue() ? CL_TRUE : CL_FALSE;
  cl_map_flags map_flags = args[3]->Uint32Value();
  size_t offset = args[4]->Uint32Value();
  size_t size = args[5]->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(args[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  void* mPtr = nullptr;
  cl_int err;
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(7) && args[7]->BooleanValue())
      eventPtr = &event;

  mPtr = clEnqueueMapBuffer(cq->getRaw(),mem->getRaw(),
                              blocking_map,map_flags, offset,
                              size, cl_events.size(),
                              NOCL_TO_CL_ARRAY(
                                cl_events, NoCLEvent),
                              eventPtr,&err);

  CHECK_ERR(err)

  if( mapPointers.count(mPtr)>0 ) {
    mapPointers[mPtr]++;
  }
  else {
    mapPointers[mPtr] = 1;
  }

  Local<Object> obj = NanNew<Object>();

  if(eventPtr) {
    obj->Set(NanNew<String>("event"), NOCL_WRAP(NoCLEvent,event));
  }
  unMapInfo* umi = new unMapInfo();
  umi->cq = cq->getRaw();
  clRetainCommandQueue(umi->cq);
  umi->mem = mem->getRaw();
  clRetainMemObject(umi->mem);

  Local<Object> buf = NanNewBufferHandle((char*)mPtr,size,freeMapedPtr,umi);
  obj->Set(NanNew<String>("buffer"), buf);

  if(!blocking_map) {
    buf->SetIndexedPropertiesToExternalArrayData(NULL, buf->GetIndexedPropertiesExternalArrayDataType(), 0);
    NoCLMapCB* cb = new NoCLMapCB(buf,size,mPtr);
    err = clSetEventCallback(event,CL_COMPLETE,notifyMapCB,cb);
    CHECK_ERR(err)
  }
  NanReturnValue(obj);

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
  NanScope();
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(mem, NoCLMem, args[1]);

  cl_bool blocking_map = args[2]->BooleanValue();
  cl_map_flags map_flags = args[3]->Uint32Value();
  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[4]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
    origin[i]=arr->Get(i)->Int32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
    region[i]=arr->Get(i)->Int32Value();

  size_t image_row_pitch;

  size_t image_slice_pitch[region[2]];

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(args[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }


  void* mPtr = nullptr;
  cl_int err;
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(7) && args[7]->BooleanValue())
      eventPtr = &event;

  mPtr = clEnqueueMapImage (cq->getRaw(),mem->getRaw(),
                              blocking_map,map_flags, origin,
                              region,
                              &image_row_pitch,image_slice_pitch,
                              cl_events.size(),
                              NOCL_TO_CL_ARRAY(
                                cl_events, NoCLEvent),
                              eventPtr,&err);

  CHECK_ERR(err)

  size_t size = image_row_pitch*region[1]*region[2];

  if( mapPointers.count(mPtr)>0 ) {
    mapPointers[mPtr]++;
  }
  else {
    mapPointers[mPtr] = 1;
  }

  Local<Object> obj = NanNew<Object>();
  obj->Set(NanNew<String>("image_row_pitch"), NanNew(static_cast<int>(image_row_pitch)));
  Local<Object> slice_pitch  = NanNewBufferHandle((char*)image_slice_pitch,sizeof(size_t)*region[2]);
  obj->Set(NanNew<String>("image_slice_pitch"), slice_pitch);

  if(eventPtr) {
    obj->Set(NanNew<String>("event"), NOCL_WRAP(NoCLEvent,event));
  }
  unMapInfo* umi = new unMapInfo();
  umi->cq = cq->getRaw();
  clRetainCommandQueue(umi->cq);
  umi->mem = mem->getRaw();
  clRetainMemObject(umi->mem);

  Local<Object> buf = NanNewBufferHandle((char*)mPtr,size,freeMapedPtr,umi);
  obj->Set(NanNew<String>("buffer"), buf);

  if(!blocking_map) {
    buf->SetIndexedPropertiesToExternalArrayData(NULL, buf->GetIndexedPropertiesExternalArrayDataType(), 0);
    NoCLMapCB* cb = new NoCLMapCB(buf,size,mPtr);
    err = clSetEventCallback(event,CL_COMPLETE,notifyMapCB,cb);
    CHECK_ERR(err)
  }
  NanReturnValue(obj);
//
//  cl_event event=nullptr;
//  if(ARG_EXISTS(9)) {
//    NOCL_UNWRAP(evt, NoCLEvent, args[9]);
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
//  NanReturnValue(NOCL_WRAP(NoCLMappedPtr, ptr));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueUnmapMemObject(cl_command_queue /* command_queue */,
//                         cl_mem           /* memobj */,
//                         void *           /* mapped_ptr */,
//                         cl_uint          /* num_events_in_wait_list */,
//                         const cl_event *  /* event_wait_list */,
//                         cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueUnmapMemObject) {
  NanScope();
  REQ_ARGS(3);
  NOCL_UNWRAP(cq, NoCLCommandQueue, args[0]);
  NOCL_UNWRAP(mem, NoCLMem, args[1]);

  void *ptr=nullptr;
  int len=0;
  if(args[2]->IsUndefined() || args[2]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE);
  }
  else {
    getPtrAndLen(args[2], ptr, len);
  }

  if(mapPointers.count(ptr) && mapPointers[ptr]>0) {
    mapPointers[ptr]--;
  }
  else {
    THROW_ERR(CL_INVALID_VALUE);
  }

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(3)) {
    Local<Array> js_events = Local<Array>::Cast(args[3]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_int err;
  //args 4
  if(ARG_EXISTS(4) && args[4]->BooleanValue()) {
    cl_event event;
    err = clEnqueueUnmapMemObject(cq->getRaw(),mem->getRaw(),ptr,
       cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),&event);
    CHECK_ERR(err)
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  }

  err = clEnqueueUnmapMemObject(cq->getRaw(),mem->getRaw(),ptr,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),nullptr);
  CHECK_ERR(err)

  Local<Object> obj = args[2].As<Object>();
  obj->SetIndexedPropertiesToExternalArrayData(NULL, obj->GetIndexedPropertiesExternalArrayDataType(), 0);

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  NanScope();
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  if(args[1]->IsNull() || args[1]->IsUndefined() || !args[1]->IsArray()) {
    THROW_ERR(CL_INVALID_VALUE);
  }

  Local<Array> arr=Local<Array>::Cast(args[1]);
  size_t num_mem_objects=arr->Length();
  unique_ptr<cl_mem[]> mem_objects(new cl_mem[num_mem_objects]);
  for(size_t i=0;i<num_mem_objects;i++) {
    Local<Value> mem=arr->Get(i);
    if (mem->IsNull() || mem->IsUndefined())
      THROW_ERR(CL_INVALID_MEM_OBJECT);
    NOCL_UNWRAP(obj, NoCLMem, mem);
    mem_objects[i]=obj->getRaw();
  }

  cl_mem_migration_flags flags=args[2]->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(3)) {
    Local<Array> js_events = Local<Array>::Cast(args[3]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(4) && args[4]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueMigrateMemObjects(
    q->getRaw(),num_mem_objects,
    mem_objects.get(),flags,
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
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
  NanScope();
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(k, NoCLKernel, args[1]);


  cl_uint work_dim=args[2]->Uint32Value();

  std::vector<size_t> cl_work_offset;
  std::vector<size_t> cl_work_global;
  std::vector<size_t> cl_work_local;


  if (ARG_EXISTS(3)) {
    Local<Array> js_work_offset = Local<Array>::Cast(args[3]);

    if (js_work_offset->Length() != work_dim) {
      THROW_ERR(CL_INVALID_GLOBAL_OFFSET);
    }

    for (unsigned int i = 0; i < work_dim; ++ i) {
      cl_work_offset.push_back(js_work_offset->Get(i)->Uint32Value());
    }
  }

  if (ARG_EXISTS(4)) {
    Local<Array> js_work_global = Local<Array>::Cast(args[4]);

    if (js_work_global->Length() != work_dim) {
      THROW_ERR(CL_INVALID_GLOBAL_WORK_SIZE);
    }

    for (unsigned int i = 0; i < work_dim; ++ i) {
      cl_work_global.push_back(js_work_global->Get(i)->Uint32Value());
    }
  }


  if (ARG_EXISTS(5)) {
    Local<Array> js_work_local = Local<Array>::Cast(args[5]);

    if (js_work_local->Length() != work_dim) {
      THROW_ERR(CL_INVALID_WORK_GROUP_SIZE);
    }

    for (unsigned int i = 0; i < work_dim; ++ i) {
      cl_work_local.push_back(js_work_local->Get(i)->Uint32Value());
    }
  }

  std::vector<NoCLEvent> cl_events;
  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(args[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && args[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueNDRangeKernel(
    q->getRaw(),
    k->getRaw(),
    1,
    cl_work_offset.size() ? cl_work_offset.data() : NULL,
    cl_work_global.size() ? cl_work_global.data() : NULL,
    cl_work_local.size() ? cl_work_local.data() : NULL,
    0, NULL, NULL
  ));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
}

#ifndef CL_VERSION_2_0
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueTask(cl_command_queue  /* command_queue */,
//               cl_kernel         /* kernel */,
//               cl_uint           /* num_events_in_wait_list */,
//               const cl_event *  /* event_wait_list */,
//               cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(EnqueueTask) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  // Arg 1
  NOCL_UNWRAP(k, NoCLKernel, args[1]);


  std::vector<NoCLEvent> cl_events;
  Local<Array> js_events;
  if (ARG_EXISTS(2)) {
    js_events = Local<Array>::Cast(args[2]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(3) && args[3]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueTask(
    q->getRaw(),k->getRaw(),
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
}
#endif


// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueNativeKernel(cl_command_queue  /* command_queue */,
//             void (CL_CALLBACK * /*user_func*/)(void *),
//                       void *            /* args */,
//                       size_t            /* cb_args */,
//                       cl_uint           /* num_mem_objects */,
//                       const cl_mem *    /* mem_list */,
//                       const void **     /* args_mem_loc */,
//                       cl_uint           /* num_events_in_wait_list */,
//                       const cl_event *  /* event_wait_list */,
//                       cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

// Note: only available if CL_EXEC_NATIVE_KERNEL capability
NAN_METHOD(EnqueueNativeKernel) {
  NanScope();

  NanThrowError("enqueueNativeKernel is not supported by Node OpenCL");

  NanReturnUndefined();
}


#ifdef CL_VERSION_1_2
// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueMarkerWithWaitList(cl_command_queue /* command_queue */,
//                             cl_uint           /* num_events_in_wait_list */,
//                             const cl_event *  /* event_wait_list */,
//                             cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(EnqueueMarkerWithWaitList) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  std::vector<NoCLEvent> cl_events;
  Local<Array> js_events = Local<Array>::Cast(args[1]);
  NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);


  if(ARG_EXISTS(2) && args[2]->BooleanValue()) {
    cl_event event;
    CHECK_ERR(::clEnqueueMarkerWithWaitList(
        q->getRaw(),
        cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
        &event));
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  }

  CHECK_ERR(::clEnqueueMarkerWithWaitList(
    q->getRaw(),
    cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),nullptr));

    NanReturnValue(JS_INT(CL_SUCCESS));

}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueBarrierWithWaitList(cl_command_queue /* command_queue */,
//                              cl_uint           /* num_events_in_wait_list */,
//                              const cl_event *  /* event_wait_list */,
//                              cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(EnqueueBarrierWithWaitList) {
    NanScope();
    REQ_ARGS(2);

    // Arg 0
    NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

    std::vector<NoCLEvent> cl_events;
    Local<Array> js_events = Local<Array>::Cast(args[1]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);


    if(ARG_EXISTS(2) && args[2]->BooleanValue()) {
      cl_event event;
      CHECK_ERR(::clEnqueueBarrierWithWaitList(
          q->getRaw(),
          cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
          &event));
      NanReturnValue(NOCL_WRAP(NoCLEvent, event));
    }

    CHECK_ERR(::clEnqueueBarrierWithWaitList(
      q->getRaw(),
      cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),nullptr));

      NanReturnValue(JS_INT(CL_SUCCESS));
}

// // Deprecated OpenCL 1.1 APIs
#elif defined(CL_VERSION_1_1)
// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clEnqueueMarker(cl_command_queue    /* command_queue */,
//                 cl_event *          /* event */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
NAN_METHOD(EnqueueMarker) {
  NanScope();
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  if(ARG_EXISTS(1) && args[1]->BooleanValue()) {
    cl_event event;

    CHECK_ERR(::clEnqueueMarker(q->getRaw(), &event));
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  }

  CHECK_ERR(::clEnqueueMarker(q->getRaw(), nullptr));
  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clEnqueueWaitForEvents(cl_command_queue /* command_queue */,
//                         cl_uint          /* num_events */,
//                         const cl_event * /* event_list */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
NAN_METHOD(EnqueueWaitForEvents) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  std::vector<NoCLEvent> cl_events;
  Local<Array> js_events = Local<Array>::Cast(args[1]);
  NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  std::vector<cl_event> events;
  for(unsigned int cpt = 0;cpt<cl_events.size();++cpt)
      events.push_back((cl_events[cpt]).getRaw());

  CHECK_ERR(::clEnqueueWaitForEvents(q->getRaw(),
        cl_events.size(),cl_events.size() ?  events.data(): nullptr));


  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clEnqueueBarrier(cl_command_queue /* command_queue */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
NAN_METHOD(EnqueueBarrier) {
  NanScope();
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, args[0]);

  CHECK_ERR(::clEnqueueBarrier(q->getRaw()));

  NanReturnValue(JS_INT(CL_SUCCESS));
}
#endif

#ifdef CL_VERSION_2_0
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

namespace CommandQueue {
void init(Handle<Object> exports)
{
#ifndef CL_VERSION_2_0
  NODE_SET_METHOD(exports, "createCommandQueue", CreateCommandQueue);
#else
  NODE_SET_METHOD(exports, "createCommandQueueWithProperties", CreateCommandQueueWithProperties);
#endif
  NODE_SET_METHOD(exports, "retainCommandQueue", RetainCommandQueue);
  NODE_SET_METHOD(exports, "releaseCommandQueue", ReleaseCommandQueue);
  NODE_SET_METHOD(exports, "getCommandQueueInfo", GetCommandQueueInfo);
  NODE_SET_METHOD(exports, "flush", Flush);
  NODE_SET_METHOD(exports, "finish", Finish);
  NODE_SET_METHOD(exports, "enqueueReadBuffer", EnqueueReadBuffer);
  NODE_SET_METHOD(exports, "enqueueReadBufferRect", EnqueueReadBufferRect);
  NODE_SET_METHOD(exports, "enqueueWriteBuffer", EnqueueWriteBuffer);
  NODE_SET_METHOD(exports, "enqueueWriteBufferRect", EnqueueWriteBufferRect);
  NODE_SET_METHOD(exports, "enqueueCopyBuffer", EnqueueCopyBuffer);
  NODE_SET_METHOD(exports, "enqueueCopyBufferRect", EnqueueCopyBufferRect);
  NODE_SET_METHOD(exports, "enqueueReadImage", EnqueueReadImage);
  NODE_SET_METHOD(exports, "enqueueWriteImage", EnqueueWriteImage);
  NODE_SET_METHOD(exports, "enqueueCopyImage", EnqueueCopyImage);
  NODE_SET_METHOD(exports, "enqueueCopyImageToBuffer", EnqueueCopyImageToBuffer);
  NODE_SET_METHOD(exports, "enqueueCopyBufferToImage", EnqueueCopyBufferToImage);
  NODE_SET_METHOD(exports, "enqueueMapBuffer", EnqueueMapBuffer);
  NODE_SET_METHOD(exports, "enqueueMapImage", EnqueueMapImage);
  NODE_SET_METHOD(exports, "enqueueUnmapMemObject", EnqueueUnmapMemObject);
  NODE_SET_METHOD(exports, "enqueueNDRangeKernel", EnqueueNDRangeKernel);
#ifndef CL_VERSION_2_0
  NODE_SET_METHOD(exports, "enqueueTask", EnqueueTask); // removed in 2.0
#endif
  NODE_SET_METHOD(exports, "enqueueNativeKernel", EnqueueNativeKernel);
#ifdef CL_VERSION_1_2
  NODE_SET_METHOD(exports, "enqueueMarkerWithWaitList", EnqueueMarkerWithWaitList);
  NODE_SET_METHOD(exports, "enqueueBarrierWithWaitList", EnqueueBarrierWithWaitList);
  NODE_SET_METHOD(exports, "enqueueFillBuffer", EnqueueFillBuffer);
  NODE_SET_METHOD(exports, "enqueueFillImage", EnqueueFillImage);
  NODE_SET_METHOD(exports, "enqueueMigrateMemObjects", EnqueueMigrateMemObjects);
#elif defined(CL_VERSION_1_1)
  NODE_SET_METHOD(exports, "enqueueWaitForEvents", EnqueueWaitForEvents);
  NODE_SET_METHOD(exports, "enqueueMarker", EnqueueMarker);
  NODE_SET_METHOD(exports, "enqueueBarrier", EnqueueBarrier);
#endif
}
} // namespace CommandQueue

} // namespace opencl
