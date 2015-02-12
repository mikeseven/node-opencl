#include "commandqueue.h"

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

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_CONTEXT));
  }
  cl_context context=Unwrap<cl_context>(args[0]);

  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_DEVICE));
  }
  cl_device_id device=Unwrap<cl_device_id>(args[1]);
  cl_command_queue_properties properties = args[2]->Uint32Value();

  cl_int err;
  cl_command_queue q = ::clCreateCommandQueue(context, device, properties, &err);
  CHECK_ERR(err)

  NanReturnValue(Wrap(q));
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


  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_CONTEXT));
  }
  cl_context context=Unwrap<cl_context>(args[0]);

  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_DEVICE));
  }
  cl_device_id device=Unwrap<cl_device_id>(args[1]);

  Local<Array> properties = Local<Array>::Cast(args[1]);
  vector<cl_queue_properties> cl_properties;

  for (uint32_t i=0; i < properties->Length(); i++) {
    cl_uint prop_id = properties->Get(i)->Uint32Value();
    cl_properties.push_back(prop_id);
    if(prop_id == CL_QUEUE_PROPERTIES) {
      cl_bool * norm = Unwrap<cl_bool *>(properties->Get(++i));
      cl_properties.push_back((cl_context_properties) * norm);
    } else if (prop_id == CL_QUEUE_SIZE) {
       cl_uint * size = Unwrap<cl_uint *>(properties->Get(++i));
      cl_properties.push_back((cl_context_properties) * size);
    } else {
      // TODO Throw good exception
      return NanThrowError(JS_INT(CL_INVALID_SAMPLER));
    }
  }


  cl_int err;
  cl_command_queue q = ::clCreateCommandQueueWithProperties(context, device, &cl_properties.front(), &err);
  CHECK_ERR(err)

  NanReturnValue(Wrap(q));
}
#endif

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainCommandQueue(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainCommandQueue) {
  NanScope();
  REQ_ARGS(1);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }

  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  cl_int count=clRetainCommandQueue(q);

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseCommandQueue(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseCommandQueue) {
  NanScope();
  REQ_ARGS(1);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }

  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  cl_int count=clReleaseCommandQueue(q);

  NanReturnValue(JS_INT(count));
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

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }

  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  cl_command_queue_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_QUEUE_CONTEXT: {
      cl_context val;
      CHECK_ERR(::clGetCommandQueueInfo(q,param_name,sizeof(cl_context), &val, nullptr))
      // TODO NanReturnValue(JS_INT(val));
      break;
    }
    case CL_QUEUE_DEVICE: {
      cl_device_id val;
      CHECK_ERR(::clGetCommandQueueInfo(q,param_name,sizeof(cl_device_id), &val, nullptr))
      // TODO NanReturnValue(JS_INT(val));
      break;
    }
    case CL_QUEUE_REFERENCE_COUNT: {
      cl_uint val;
      CHECK_ERR(::clGetCommandQueueInfo(q,param_name,sizeof(cl_uint), &val, nullptr))
      NanReturnValue(JS_INT(val));
    }
    case CL_QUEUE_PROPERTIES: {
      cl_command_queue_properties val;
      CHECK_ERR(::clGetCommandQueueInfo(q,param_name,sizeof(cl_command_queue_properties), &val, nullptr))
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

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }

  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  CHECK_ERR(::clFlush(q));
  NanReturnUndefined();
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clFinish(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(Finish) {
  NanScope();
  REQ_ARGS(1);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }

  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  CHECK_ERR(::clFinish(q));
  NanReturnUndefined();
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
  REQ_ARGS(8);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem buffer = Unwrap<cl_mem>(args[1]);
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

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[6]), events);

  cl_event event=nullptr;
  if(!args[7]->IsUndefined() && !args[7]->IsNull()) {
    event=Unwrap<cl_event>(args[7]);
  }

  CHECK_ERR(::clEnqueueReadBuffer(q,buffer,blocking_read,offset,size,ptr,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));
  NanReturnValue(JS_INT(CL_SUCCESS));
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
  REQ_ARGS(13);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem buffer = Unwrap<cl_mem>(args[1]);
  cl_bool blocking_read = args[2]->BooleanValue();

  size_t buffer_offset[]={0,0,0};
  size_t host_offset[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      buffer_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<arr->Length();i++)
      host_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<arr->Length();i++)
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

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[11]), events);

  cl_event event=nullptr;
  if(!args[12]->IsUndefined() && !args[12]->IsNull()) {
    event=Unwrap<cl_event>(args[12]);
  }

  CHECK_ERR(::clEnqueueReadBufferRect(q,buffer,blocking_read,buffer_offset,host_offset,region,
    buffer_row_pitch,buffer_slice_pitch,host_row_pitch,host_slice_pitch,ptr,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  REQ_ARGS(8);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem buffer = Unwrap<cl_mem>(args[1]);
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

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[6]), events);

  cl_event event=nullptr;
  if(!args[7]->IsUndefined() && !args[7]->IsNull()) {
    event=Unwrap<cl_event>(args[7]);
  }

  CHECK_ERR(::clEnqueueWriteBuffer(q,buffer,blocking_write,offset,size,ptr,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  REQ_ARGS(13);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem buffer = Unwrap<cl_mem>(args[1]);
  cl_bool blocking_write = args[2]->BooleanValue();

  size_t buffer_offset[]={0,0,0};
  size_t host_offset[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      buffer_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<arr->Length();i++)
      host_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<arr->Length();i++)
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

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[11]), events);

  cl_event event=nullptr;
  if(!args[12]->IsUndefined() && !args[12]->IsNull()) {
    event=Unwrap<cl_event>(args[12]);
  }

  CHECK_ERR(::clEnqueueWriteBufferRect(q,buffer,blocking_write,buffer_offset,host_offset,region,
    buffer_row_pitch,buffer_slice_pitch,host_row_pitch,host_slice_pitch,ptr,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

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
  REQ_ARGS(7);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem buffer = Unwrap<cl_mem>(args[1]);

  void *pattern=nullptr;
  int len=0;
  if(args[2]->IsUndefined() || args[2]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[2],pattern,len);

  size_t offset=args[3]->Uint32Value();
  size_t size=args[4]->Uint32Value();

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[11]), events);

  cl_event event=nullptr;
  if(!args[6]->IsUndefined() && !args[6]->IsNull()) {
    event=Unwrap<cl_event>(args[6]);
  }

  CHECK_ERR(::clEnqueueFillBuffer(q,buffer,pattern, len, offset, size,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

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
  REQ_ARGS(7);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem src_buffer = Unwrap<cl_mem>(args[1]);
  if(!isOpenCLObj(args[2])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem dst_buffer = Unwrap<cl_mem>(args[2]);

  size_t src_offset=args[3]->Uint32Value();
  size_t dst_offset=args[4]->Uint32Value();
  size_t size=args[5]->Uint32Value();

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[6]), events);

  cl_event event=nullptr;
  if(!args[7]->IsUndefined() && !args[7]->IsNull()) {
    event=Unwrap<cl_event>(args[7]);
  }

  CHECK_ERR(::clEnqueueCopyBuffer(q,
    src_buffer,dst_buffer,src_offset,dst_offset, size,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));
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
  REQ_ARGS(13);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);

  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem src_buffer = Unwrap<cl_mem>(args[1]);

  if(!isOpenCLObj(args[2])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem dst_buffer = Unwrap<cl_mem>(args[2]);

  size_t src_origin[]={0,0,0};
  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      src_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<arr->Length();i++)
      dst_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<arr->Length();i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t src_row_pitch = args[6]->Uint32Value();
  size_t src_slice_pitch = args[7]->Uint32Value();
  size_t dst_row_pitch = args[8]->Uint32Value();
  size_t dst_slice_pitch = args[9]->Uint32Value();

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[10]), events);

  cl_event event=nullptr;
  if(!args[11]->IsUndefined() && !args[11]->IsNull()) {
    event=Unwrap<cl_event>(args[11]);
  }

  CHECK_ERR(::clEnqueueCopyBufferRect(q,src_buffer,dst_buffer,
    src_origin, dst_origin, region,
    src_row_pitch, src_slice_pitch, dst_row_pitch, dst_slice_pitch,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  REQ_ARGS(10);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem image = Unwrap<cl_mem>(args[1]);
  cl_bool blocking_read = args[2]->BooleanValue();

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<arr->Length();i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t row_pitch = args[5]->Uint32Value();
  size_t slice_pitch = args[6]->Uint32Value();

  void *ptr=nullptr;
  int len=0;
  if(args[7]->IsUndefined() || args[7]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[7],ptr,len);

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[8]), events);

  cl_event event=nullptr;
  if(!args[9]->IsUndefined() && !args[9]->IsNull()) {
    event=Unwrap<cl_event>(args[9]);
  }

  CHECK_ERR(::clEnqueueReadImage(q,image,blocking_read,
    origin,region,row_pitch,slice_pitch, ptr,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  REQ_ARGS(10);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem image = Unwrap<cl_mem>(args[1]);
  cl_bool blocking_write = args[2]->BooleanValue();

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<arr->Length();i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t row_pitch = args[5]->Uint32Value();
  size_t slice_pitch = args[6]->Uint32Value();

  void *ptr=nullptr;
  int len=0;
  if(args[7]->IsUndefined() || args[7]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[7],ptr,len);

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[8]), events);

  cl_event event=nullptr;
  if(!args[9]->IsUndefined() && !args[9]->IsNull()) {
    event=Unwrap<cl_event>(args[9]);
  }

  CHECK_ERR(::clEnqueueWriteImage(q,image,blocking_write,
    origin,region,row_pitch,slice_pitch, ptr,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

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
  REQ_ARGS(7);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem image = Unwrap<cl_mem>(args[1]);

  void *fill_color=nullptr;
  int len=0;
  if(args[2]->IsUndefined() || args[2]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[2],fill_color,len);

  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<arr->Length();i++)
      region[i]=arr->Get(i)->Uint32Value();

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[5]), events);

  cl_event event=nullptr;
  if(!args[6]->IsUndefined() && !args[6]->IsNull()) {
    event=Unwrap<cl_event>(args[6]);
  }

  CHECK_ERR(::clEnqueueFillImage(q,image,fill_color,
    origin,region,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

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
  REQ_ARGS(8);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem src_image = Unwrap<cl_mem>(args[1]);
  if(!isOpenCLObj(args[2])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem dst_image = Unwrap<cl_mem>(args[2]);

  size_t src_origin[]={0,0,0};
  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      src_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<arr->Length();i++)
      dst_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<arr->Length();i++)
      region[i]=arr->Get(i)->Uint32Value();

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[6]), events);

  cl_event event=nullptr;
  if(!args[7]->IsUndefined() && !args[7]->IsNull()) {
    event=Unwrap<cl_event>(args[7]);
  }

  CHECK_ERR(::clEnqueueCopyImage(q,src_image,dst_image,
    src_origin,dst_origin, region,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  REQ_ARGS(8);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem src_image = Unwrap<cl_mem>(args[1]);
  if(!isOpenCLObj(args[2])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem dst_buffer = Unwrap<cl_mem>(args[2]);

  size_t src_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      src_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<arr->Length();i++)
      region[i]=arr->Get(i)->Uint32Value();

  size_t dst_offset = args[5]->Uint32Value();

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[6]), events);

  cl_event event=nullptr;
  if(!args[7]->IsUndefined() && !args[7]->IsNull()) {
    event=Unwrap<cl_event>(args[7]);
  }

  CHECK_ERR(::clEnqueueCopyImageToBuffer(q,src_image,dst_buffer,
    src_origin, region, dst_offset,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  REQ_ARGS(8);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem src_buffer = Unwrap<cl_mem>(args[1]);
  if(!isOpenCLObj(args[2])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem dst_image = Unwrap<cl_mem>(args[2]);

  size_t src_offset = args[3]->Uint32Value();

  size_t dst_origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[4]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      dst_origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<arr->Length();i++)
      region[i]=arr->Get(i)->Uint32Value();

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[6]), events);

  cl_event event=nullptr;
  if(!args[7]->IsUndefined() && !args[7]->IsNull()) {
    event=Unwrap<cl_event>(args[7]);
  }

  CHECK_ERR(::clEnqueueCopyBufferToImage(q,src_buffer,dst_image,
    src_offset, dst_origin, region,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
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

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem buffer = Unwrap<cl_mem>(args[1]);

  cl_bool blocking_map = args[2]->BooleanValue();
  cl_map_flags map_flags = args[3]->BooleanValue();
  size_t offset = args[4]->Uint32Value();
  size_t size = args[5]->Uint32Value();

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[6]), events);

  cl_event event=nullptr;
  if(!args[7]->IsUndefined() && !args[7]->IsNull()) {
    event=Unwrap<cl_event>(args[7]);
  }

  cl_int ret=CL_SUCCESS;
  void *ptr = ::clEnqueueMapBuffer(q,buffer,blocking_map,
    map_flags,offset, size,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr,
    &ret);

  CHECK_ERR(ret);

  NanReturnValue(Wrap(ptr));
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
  REQ_ARGS(10);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem image = Unwrap<cl_mem>(args[1]);

  cl_bool blocking_map = args[2]->BooleanValue();
  cl_map_flags map_flags = args[3]->BooleanValue();
  size_t origin[]={0,0,0};
  size_t region[]={1,1,1};
  Local<Array> arr= Local<Array>::Cast(args[4]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      origin[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<arr->Length();i++)
      region[i]=arr->Get(i)->Uint32Value();

  void *image_row_pitch=nullptr;
  int len_image_row_pitch=0;
  if(args[6]->IsUndefined() || args[6]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[6],image_row_pitch,len_image_row_pitch);

  void *image_slice_pitch=nullptr;
  int len_image_slice_pitch=0;
  if(args[7]->IsUndefined() || args[7]->IsNull()) {
    CHECK_ERR(CL_INVALID_VALUE);
  }
  else
    getPtrAndLen(args[7],image_slice_pitch,len_image_slice_pitch);

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[8]), events);

  cl_event event=nullptr;
  if(!args[9]->IsUndefined() && !args[9]->IsNull()) {
    event=Unwrap<cl_event>(args[9]);
  }

  cl_int ret=CL_SUCCESS;
  void *ptr = ::clEnqueueMapImage(q,image,blocking_map,map_flags,
    origin, region, (size_t*)image_row_pitch, (size_t*)image_slice_pitch,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr,
    &ret);

  CHECK_ERR(ret);

  NanReturnValue(Wrap(ptr));
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
  REQ_ARGS(5);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_mem memobj = Unwrap<cl_mem>(args[1]);

  void *mapped_ptr=Unwrap<void*>(args[2]);

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[3]), events);

  cl_event event=nullptr;
  if(!args[4]->IsUndefined() && !args[4]->IsNull()) {
    event=Unwrap<cl_event>(args[4]);
  }

  CHECK_ERR(::clEnqueueUnmapMemObject(q,memobj,
    mapped_ptr,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

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
  REQ_ARGS(5);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);

  if(args[1]->IsNull() || args[1]->IsUndefined() || !args[1]->IsArray()) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }

  Local<Array> arr=Local<Array>::Cast(args[1]);
  size_t num_mem_objects=arr->Length();
  unique_ptr<cl_mem[]> mem_objects(new cl_mem[num_mem_objects]);
  for(size_t i=0;i<num_mem_objects;i++) {
    Local<Value> mem=arr->Get(i);
    if(!isOpenCLObj(mem)) {
      return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
    }
    cl_mem obj=Unwrap<cl_mem>(mem);
    mem_objects[i]=obj;
  }

  cl_mem_migration_flags flags=args[2]->Uint32Value();

 std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[3]), events);

  cl_event event=nullptr;
  if(!args[4]->IsUndefined() && !args[4]->IsNull()) {
    event=Unwrap<cl_event>(args[4]);
  }

  CHECK_ERR(::clEnqueueMigrateMemObjects(q,num_mem_objects,
    mem_objects.get(),flags,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

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
  REQ_ARGS(8);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_KERNEL));
  }
  cl_kernel k = Unwrap<cl_kernel>(args[1]);

  cl_uint work_dim=args[2]->Uint32Value();
  size_t global_work_offset[]={0,0,0};
  size_t global_work_size[]={0,0,0};
  size_t local_work_size[]={0,0,0};
  Local<Array> arr= Local<Array>::Cast(args[3]);
  uint32_t i;
  for(i=0;i<arr->Length();i++)
      global_work_offset[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[4]);
  for(i=0;i<arr->Length();i++)
      global_work_size[i]=arr->Get(i)->Uint32Value();
  arr= Local<Array>::Cast(args[5]);
  for(i=0;i<arr->Length();i++)
      local_work_size[i]=arr->Get(i)->Uint32Value();

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[6]), events);

  cl_event event=nullptr;
  if(!args[7]->IsUndefined() && !args[7]->IsNull()) {
    event=Unwrap<cl_event>(args[7]);
  }

  CHECK_ERR(::clEnqueueNDRangeKernel(q,k,work_dim,
    global_work_offset,global_work_size,local_work_size,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  REQ_ARGS(4);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_KERNEL));
  }
  cl_kernel k = Unwrap<cl_kernel>(args[1]);

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[2]), events);

  cl_event event=nullptr;
  if(!args[3]->IsUndefined() && !args[3]->IsNull()) {
    event=Unwrap<cl_event>(args[3]);
  }

  CHECK_ERR(::clEnqueueTask(q,k,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  // TODO
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
  REQ_ARGS(3);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[1]), events);

  cl_event event=nullptr;
  if(!args[2]->IsUndefined() && !args[2]->IsNull()) {
    event=Unwrap<cl_event>(args[2]);
  }

  CHECK_ERR(::clEnqueueMarkerWithWaitList(q,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clEnqueueBarrierWithWaitList(cl_command_queue /* command_queue */,
//                              cl_uint           /* num_events_in_wait_list */,
//                              const cl_event *  /* event_wait_list */,
//                              cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(EnqueueBarrierWithWaitList) {
  NanScope();
  REQ_ARGS(3);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[1]), events);

  cl_event event=nullptr;
  if(!args[2]->IsUndefined() && !args[2]->IsNull()) {
    event=Unwrap<cl_event>(args[2]);
  }

  CHECK_ERR(::clEnqueueBarrierWithWaitList(q,
    events.size(),events.size() ? &events.front() : nullptr,
    event ? &event : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

// // Deprecated OpenCL 1.1 APIs
#elif defined(CL_VERSION_1_1)
// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clEnqueueMarker(cl_command_queue    /* command_queue */,
//                 cl_event *          /* event */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
NAN_METHOD(EnqueueMarker) {
  NanScope();
  REQ_ARGS(2);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);

  cl_event event=nullptr;
  if(!args[1]->IsUndefined() && !args[1]->IsNull()) {
    event=Unwrap<cl_event>(args[1]);
  }

  CHECK_ERR(::clEnqueueMarker(q, &event));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clEnqueueWaitForEvents(cl_command_queue /* command_queue */,
//                         cl_uint          /* num_events */,
//                         const cl_event * /* event_list */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
NAN_METHOD(EnqueueWaitForEvents) {
  NanScope();
  REQ_ARGS(2);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);

  std::vector<cl_event> events;
  getValuesFromArray(Local<Array>::Cast(args[1]), events);

  CHECK_ERR(::clEnqueueWaitForEvents(q,
        events.size(),events.size() ? &events.front() : nullptr));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clEnqueueBarrier(cl_command_queue /* command_queue */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
NAN_METHOD(EnqueueBarrier) {
  NanScope();
  REQ_ARGS(1);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_COMMAND_QUEUE));
  }
  cl_command_queue q = Unwrap<cl_command_queue>(args[0]);

  CHECK_ERR(::clEnqueueBarrier(q));

  NanReturnValue(JS_INT(CL_SUCCESS));
}

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
  NODE_SET_METHOD(exports, "enqueueFillBuffer", EnqueueFillBuffer);
  NODE_SET_METHOD(exports, "enqueueCopyBuffer", EnqueueCopyBuffer);
  NODE_SET_METHOD(exports, "enqueueCopyBufferRect", EnqueueCopyBufferRect);
  NODE_SET_METHOD(exports, "enqueueReadImage", EnqueueReadImage);
  NODE_SET_METHOD(exports, "enqueueWriteImage", EnqueueWriteImage);
  NODE_SET_METHOD(exports, "enqueueFillImage", EnqueueFillImage);
  NODE_SET_METHOD(exports, "enqueueCopyImage", EnqueueCopyImage);
  NODE_SET_METHOD(exports, "enqueueCopyImageToBuffer", EnqueueCopyImageToBuffer);
  NODE_SET_METHOD(exports, "enqueueCopyBufferToImage", EnqueueCopyBufferToImage);
  NODE_SET_METHOD(exports, "enqueueMapBuffer", EnqueueMapBuffer);
  NODE_SET_METHOD(exports, "enqueueMapImage", EnqueueMapImage);
  NODE_SET_METHOD(exports, "enqueueUnmapMemObject", EnqueueUnmapMemObject);
  NODE_SET_METHOD(exports, "enqueueMigrateMemObjects", EnqueueMigrateMemObjects);
  NODE_SET_METHOD(exports, "enqueueNDRangeKernel", EnqueueNDRangeKernel);
#ifndef CL_VERSION_2_0
  NODE_SET_METHOD(exports, "enqueueTask", EnqueueTask); // removed in 2.0
#endif
  NODE_SET_METHOD(exports, "enqueueNativeKernel", EnqueueNativeKernel);
#ifdef CL_VERSION_1_2
  NODE_SET_METHOD(exports, "enqueueMarkerWithWaitList", EnqueueMarkerWithWaitList);
  NODE_SET_METHOD(exports, "enqueueBarrierWithWaitList", EnqueueBarrierWithWaitList);
#elif defined(CL_VERSION_1_1)
  NODE_SET_METHOD(exports, "enqueueMarker", EnqueueMarker);
  NODE_SET_METHOD(exports, "EnqueueWaitForEvents", EnqueueWaitForEvents);
  NODE_SET_METHOD(exports, "enqueueBarrier", EnqueueBarrier);
#endif
}
} // namespace CommandQueue

} // namespace opencl
