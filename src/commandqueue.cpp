#include <algorithm>
#include <memory>
// #include <unordered_map>
#include "commandqueue.h"
#include "types.h"
#include "nanextension.h"

namespace opencl {

#ifndef CL_VERSION_2_0

// /* Command Queue APIs */
// extern CL_API_ENTRY cl_command_queue CL_API_CALL
// clCreateCommandQueue(cl_context                     /* context */,
//                      cl_device_id                   /* device */,
//                      cl_command_queue_properties    /* properties */,
//                      cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateCommandQueue) {
  Nan::HandleScope scope;
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 1
  NOCL_UNWRAP(device, NoCLDeviceId, info[1]);

  cl_command_queue_properties properties = info[2]->Uint32Value();

  cl_int err;
  cl_command_queue q = ::clCreateCommandQueue(
    context->getRaw(), device->getRaw(), properties, &err);

  CHECK_ERR(err)
  info.GetReturnValue().Set(NOCL_WRAP(NoCLCommandQueue, q));
}

#else

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

  cl_command_queue_info param_name = info[1]->Uint32Value();

  switch(param_name) {
    case CL_QUEUE_CONTEXT: {
      cl_context val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_context), &val, nullptr))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLContext, val));
      return;
    }
    case CL_QUEUE_DEVICE: {
      cl_device_id val;
      CHECK_ERR(::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_device_id), &val, nullptr))
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

  cl_bool blocking_read = info[2]->BooleanValue();
  size_t offset = info[3]->Uint32Value();
  size_t size = info[4]->Uint32Value();

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

  std::vector<NoCLEvent*> cl_events;

  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(info[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  //::clGetCommandQueueInfo(q->getRaw(),param_name,sizeof(cl_context), &val, nullptr)
  cl_event event=nullptr;
  if(ARG_EXISTS(7) && info[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueReadBuffer(
    q->getRaw(),buffer->getRaw(),blocking_read,offset,size,ptr,
    (cl_uint) cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(11);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, info[1]);

  // Arg 2
  cl_bool blocking_read = info[2]->BooleanValue();

  size_t buffer_offset[]={0,0,0};
  size_t host_offset[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      buffer_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      host_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t buffer_row_pitch = info[6]->Uint32Value();
  size_t buffer_slice_pitch = info[7]->Uint32Value();
  size_t host_row_pitch = info[8]->Uint32Value();
  size_t host_slice_pitch = info[9]->Uint32Value();

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

  std::vector<NoCLEvent *> cl_events;

  if (ARG_EXISTS(11)) {
    Local<Array> js_events = Local<Array>::Cast(info[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(12) && info[12]->BooleanValue()) {
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
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, info[1]);

  cl_bool blocking_write = info[2]->BooleanValue();
  size_t offset = info[3]->Uint32Value();
  size_t size = info[4]->Uint32Value();

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

  std::vector<NoCLEvent *> cl_events;

  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(info[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);

  }
  cl_event event=nullptr;
  if(ARG_EXISTS(7) && info[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueWriteBuffer(
    q->getRaw(),buffer->getRaw(),blocking_write,offset,size,ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(11);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(buffer, NoCLMem, info[1]);

  // Arg 2
  cl_bool blocking_write = info[2]->BooleanValue();

  size_t buffer_offset[]={0,0,0};
  size_t host_offset[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      buffer_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      host_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t buffer_row_pitch = info[6]->Uint32Value();
  size_t buffer_slice_pitch = info[7]->Uint32Value();
  size_t host_row_pitch = info[8]->Uint32Value();
  size_t host_slice_pitch = info[9]->Uint32Value();

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

  std::vector<NoCLEvent *> cl_events;
  if (ARG_EXISTS(12)) {
    Local<Array> js_events = Local<Array>::Cast(info[11]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }


  cl_event event=nullptr;
  if(ARG_EXISTS(12) && info[12]->BooleanValue()) {
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
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
    scalar_pattern_int = info[2]->Int32Value();
    pattern = &scalar_pattern_int;
    len = sizeof(cl_uint);
  }
  else if (info[2]->IsNumber()) {
    scalar_pattern_double = info[2]->NumberValue();
    pattern = &scalar_pattern_double;
    len = sizeof(cl_double);
  }
  else {
    getPtrAndLen(info[2], pattern, len);
  }
  if(!pattern || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  size_t offset=info[3]->Uint32Value();
  size_t size=info[4]->Uint32Value();

  std::vector<NoCLEvent *> cl_events;
  if (ARG_EXISTS(5)) {
    Local<Array> js_events = Local<Array>::Cast(info[5]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(6) && info[6]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueFillBuffer(
    q->getRaw(), buffer->getRaw(), pattern, len, offset, size,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(src_buffer, NoCLMem, info[1]);

  // Arg 2
  NOCL_UNWRAP(dst_buffer, NoCLMem, info[2]);


  size_t src_offset=info[3]->Uint32Value();
  size_t dst_offset=info[4]->Uint32Value();
  size_t size=info[5]->Uint32Value();

  std::vector<NoCLEvent *> cl_events;

  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(info[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && info[7]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueCopyBuffer(q->getRaw(),
    src_buffer->getRaw(),dst_buffer->getRaw(),src_offset,dst_offset, size,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent), &event));
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
      src_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      dst_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();
  size_t src_row_pitch = info[6]->Uint32Value();
  size_t src_slice_pitch = info[7]->Uint32Value();
  size_t dst_row_pitch = info[8]->Uint32Value();
  size_t dst_slice_pitch = info[9]->Uint32Value();

  std::vector<NoCLEvent *> cl_events;
  if(ARG_EXISTS(10)) {
    Local<Array> js_events = Local<Array>::Cast(info[10]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(11) && info[11]->BooleanValue()) {
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
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(8);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(image, NoCLMem, info[1]);

  cl_bool blocking_read = info[2]->BooleanValue();

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t row_pitch = info[5]->Uint32Value();
  size_t slice_pitch = info[6]->Uint32Value();

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

  std::vector<NoCLEvent *> cl_events;
  if(ARG_EXISTS(8)) {
    Local<Array> js_events = Local<Array>::Cast(info[8]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(9) && info[9]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueReadImage(q->getRaw(),image->getRaw(),blocking_read,
    origin,region,row_pitch,slice_pitch, ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(8);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(image, NoCLMem, info[1]);

  // Arg 2
  cl_bool blocking_write = info[2]->BooleanValue();

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[3]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t row_pitch = info[5]->Uint32Value();
  size_t slice_pitch = info[6]->Uint32Value();

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

  std::vector<NoCLEvent *> cl_events;
  if (ARG_EXISTS(8)) {
    Local<Array> js_events = Local<Array>::Cast(info[8]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(9) && info[9]->BooleanValue()) {
    cl_context ctx;
    cl_int err = 0;
    err = ::clGetCommandQueueInfo(q->getRaw(), CL_QUEUE_CONTEXT, sizeof(cl_context), &ctx, NULL);
    CHECK_ERR(err);

    event = ::clCreateUserEvent(ctx, &err);
    CHECK_ERR(err);
  }

  CHECK_ERR(::clEnqueueWriteImage(q->getRaw(),image->getRaw(),blocking_write,
    origin,region,row_pitch,slice_pitch, ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  std::vector<NoCLEvent *> cl_events;
  if(ARG_EXISTS(5)) {
    Local<Array> js_events = Local<Array>::Cast(info[5]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(6) && info[6]->BooleanValue()) {
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
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
      src_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      dst_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  std::vector<NoCLEvent *> cl_events;
  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(info[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && info[7]->BooleanValue()) {
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
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
      src_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[4]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t dst_offset = info[5]->Uint32Value();

  std::vector<NoCLEvent *> cl_events;
  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(info[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && info[7]->BooleanValue()) {
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
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(src_buffer, NoCLMem, info[1]);

  // Arg 2
  NOCL_UNWRAP(dst_image, NoCLMem, info[2]);

  size_t src_offset = info[3]->Uint32Value();

  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[4]);
  uint32_t i;

  for(i=0;i<max(arr->Length(),2u);i++)
      dst_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
      region[i]=arr->Get(i)->Uint32Value();

  std::vector<NoCLEvent *> cl_events;
  if(ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(info[7]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && info[7]->BooleanValue()) {
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
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
  }
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

  cl_bool blocking_map = info[2]->BooleanValue() ? CL_TRUE : CL_FALSE;
  cl_map_flags map_flags = info[3]->Uint32Value();
  size_t offset = info[4]->Uint32Value();
  size_t size = info[5]->Uint32Value();

  std::vector<NoCLEvent *> cl_events;
  if(ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(info[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  void* mPtr = nullptr;
  cl_int err;
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(7) && info[7]->BooleanValue())
      eventPtr = &event;

  mPtr = clEnqueueMapBuffer(cq->getRaw(),mem->getRaw(),
                            blocking_map,map_flags, offset,
                            size, (cl_uint)cl_events.size(),
                            NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
                            eventPtr,&err);

  CHECK_ERR(err)

  Local<v8::ArrayBuffer> obj = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), mPtr, size);

  if(eventPtr) {
    obj->Set(JS_STR("event"), NOCL_WRAP(NoCLEvent,event));
  }

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

  cl_bool blocking_map = info[2]->BooleanValue();
  cl_map_flags map_flags = info[3]->Uint32Value();
  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(info[4]);
  uint32_t i;
  for(i=0;i<max(arr->Length(),2u);i++)
    origin[i]=arr->Get(i)->Int32Value();
  arr= Local<Array>::Cast(info[5]);
  for(i=0;i<max(arr->Length(),2u);i++)
    region[i]=arr->Get(i)->Int32Value();

  size_t image_row_pitch;

  std::unique_ptr<size_t[]> image_slice_pitch(new size_t[region[2]]);

  std::vector<NoCLEvent *> cl_events;
  if(ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(info[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }


  void* mPtr = nullptr;
  cl_int err;
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(7) && info[7]->BooleanValue())
      eventPtr = &event;

  mPtr = clEnqueueMapImage (cq->getRaw(),mem->getRaw(),
                              blocking_map,map_flags, origin,
                              region,
                              &image_row_pitch, image_slice_pitch.get(),
                              (cl_uint)cl_events.size(),
                              NOCL_TO_CL_ARRAY(
                                cl_events, NoCLEvent),
                                eventPtr,&err);

  CHECK_ERR(err)

  size_t size = image_row_pitch*region[1]*region[2];

  Local<v8::ArrayBuffer> obj = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), mPtr, size);

  obj->Set(JS_STR("image_row_pitch"), Nan::New(static_cast<int>(image_row_pitch)));
  Local<Object> slice_pitch  = Nan::NewBuffer((char*)image_slice_pitch.get(), (uint32_t) (sizeof(size_t)*region[2])).ToLocalChecked();
  obj->Set(JS_STR("image_slice_pitch"), slice_pitch);

  if(eventPtr) {
    obj->Set(JS_STR("event"), NOCL_WRAP(NoCLEvent,event));
  }

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
  NOCL_UNWRAP(cq, NoCLCommandQueue, info[0]);
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

  std::vector<NoCLEvent *> cl_events;
  if(ARG_EXISTS(3)) {
    Local<Array> js_events = Local<Array>::Cast(info[3]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_int err;

  if(ARG_EXISTS(4) && info[4]->BooleanValue()) {
    cl_event event;
    err = clEnqueueUnmapMemObject(cq->getRaw(),mem->getRaw(),ptr,
      (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),&event);
    CHECK_ERR(err)
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
    return;
  }

  err = clEnqueueUnmapMemObject(cq->getRaw(),mem->getRaw(),ptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),nullptr);
  CHECK_ERR(err)

  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
    Local<Value> mem=arr->Get(i);
    if (mem->IsNull() || mem->IsUndefined())
      THROW_ERR(CL_INVALID_MEM_OBJECT);
    NOCL_UNWRAP(obj, NoCLMem, mem);
    mem_objects[i]=obj->getRaw();
  }

  cl_mem_migration_flags flags=info[2]->Uint32Value();

  std::vector<NoCLEvent *> cl_events;
  if(ARG_EXISTS(3)) {
    Local<Array> js_events = Local<Array>::Cast(info[3]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(4) && info[4]->BooleanValue()) {
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
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(6);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(k, NoCLKernel, info[1]);


  cl_uint work_dim=info[2]->Uint32Value();

  std::vector<size_t> cl_work_offset;
  std::vector<size_t> cl_work_global;
  std::vector<size_t> cl_work_local;


  if (ARG_EXISTS(3)) {
    Local<Array> js_work_offset = Local<Array>::Cast(info[3]);

    if (js_work_offset->Length() != work_dim) {
      THROW_ERR(CL_INVALID_GLOBAL_OFFSET);
    }

    for (unsigned int i = 0; i < work_dim; ++ i) {
      cl_work_offset.push_back(js_work_offset->Get(i)->Uint32Value());
    }
  }

  if (ARG_EXISTS(4)) {
    Local<Array> js_work_global = Local<Array>::Cast(info[4]);

    if (js_work_global->Length() != work_dim) {
      THROW_ERR(CL_INVALID_GLOBAL_WORK_SIZE);
    }

    for (unsigned int i = 0; i < work_dim; ++ i) {
      cl_work_global.push_back(js_work_global->Get(i)->Uint32Value());
    }
  }


  if (ARG_EXISTS(5)) {
    Local<Array> js_work_local = Local<Array>::Cast(info[5]);

    if (js_work_local->Length() != work_dim) {
      THROW_ERR(CL_INVALID_WORK_GROUP_SIZE);
    }

    for (unsigned int i = 0; i < work_dim; ++ i) {
      cl_work_local.push_back(js_work_local->Get(i)->Uint32Value());
    }
  }

  std::vector<NoCLEvent *> cl_events;
  if (ARG_EXISTS(6)) {
    Local<Array> js_events = Local<Array>::Cast(info[6]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(7) && info[7]->BooleanValue()) {
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
    work_dim,
    cl_work_offset.size() ? cl_work_offset.data() : nullptr,
    cl_work_global.size() ? cl_work_global.data() : nullptr,
    cl_work_local.size() ? cl_work_local.data() : nullptr,
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
    event ? &event : nullptr
  ));

  if (event != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  // Arg 1
  NOCL_UNWRAP(k, NoCLKernel, info[1]);


  std::vector<NoCLEvent *> cl_events;
  Local<Array> js_events;
  if (ARG_EXISTS(2)) {
    js_events = Local<Array>::Cast(info[2]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }

  cl_event event=nullptr;
  if(ARG_EXISTS(3) && info[3]->BooleanValue()) {
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
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
  }
}
#endif


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

  std::vector<NoCLEvent *> cl_events;
  Local<Array> js_events = Local<Array>::Cast(info[1]);
  NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);


  if(ARG_EXISTS(2) && info[2]->BooleanValue()) {
    cl_event event;
    CHECK_ERR(::clEnqueueMarkerWithWaitList(
        q->getRaw(),
      (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
        &event));
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
    return;
  }

  CHECK_ERR(::clEnqueueMarkerWithWaitList(
    q->getRaw(),
    (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),nullptr));

    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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

    std::vector<NoCLEvent *> cl_events;
    Local<Array> js_events = Local<Array>::Cast(info[1]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);


    if(ARG_EXISTS(2) && info[2]->BooleanValue()) {
      cl_event event;
      CHECK_ERR(::clEnqueueBarrierWithWaitList(
          q->getRaw(),
        (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
          &event));
      info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
      return;
    }

    CHECK_ERR(::clEnqueueBarrierWithWaitList(
      q->getRaw(),
      (cl_uint)cl_events.size(), NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),nullptr));

      info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// // Deprecated OpenCL 1.1 APIs
#elif defined(CL_VERSION_1_1)
// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clEnqueueMarker(cl_command_queue    /* command_queue */,
//                 cl_event *          /* event */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
NAN_METHOD(EnqueueMarker) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  if(ARG_EXISTS(1) && info[1]->BooleanValue()) {
    cl_event event;

    CHECK_ERR(::clEnqueueMarker(q->getRaw(), &event));
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
    return;
  }

  CHECK_ERR(::clEnqueueMarker(q->getRaw(), nullptr));
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clEnqueueWaitForEvents(cl_command_queue /* command_queue */,
//                         cl_uint          /* num_events */,
//                         const cl_event * /* event_list */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
NAN_METHOD(EnqueueWaitForEvents) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(q, NoCLCommandQueue, info[0]);

  std::vector<NoCLEvent *> cl_events;
  Local<Array> js_events = Local<Array>::Cast(info[1]);
  NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  std::vector<cl_event> events;
  for(unsigned int cpt = 0;cpt<cl_events.size();++cpt)
      events.push_back((cl_events[cpt])->getRaw());

  CHECK_ERR(::clEnqueueWaitForEvents(q->getRaw(),
        cl_events.size(),cl_events.size() ?  events.data(): nullptr));


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

namespace CommandQueue {
NAN_MODULE_INIT(init)
{
#ifndef CL_VERSION_2_0
  Nan::SetMethod(target, "createCommandQueue", CreateCommandQueue);
#else
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
#ifndef CL_VERSION_2_0
  Nan::SetMethod(target, "enqueueTask", EnqueueTask); // removed in 2.0
#endif
  Nan::SetMethod(target, "enqueueNativeKernel", EnqueueNativeKernel);
#ifdef CL_VERSION_1_2
  Nan::SetMethod(target, "enqueueMarkerWithWaitList", EnqueueMarkerWithWaitList);
  Nan::SetMethod(target, "enqueueBarrierWithWaitList", EnqueueBarrierWithWaitList);
  Nan::SetMethod(target, "enqueueFillBuffer", EnqueueFillBuffer);
  Nan::SetMethod(target, "enqueueFillImage", EnqueueFillImage);
  Nan::SetMethod(target, "enqueueMigrateMemObjects", EnqueueMigrateMemObjects);
#elif defined(CL_VERSION_1_1)
  Nan::SetMethod(target, "enqueueWaitForEvents", EnqueueWaitForEvents);
  Nan::SetMethod(target, "enqueueMarker", EnqueueMarker);
  Nan::SetMethod(target, "enqueueBarrier", EnqueueBarrier);
#endif
}
} // namespace CommandQueue

} // namespace opencl
