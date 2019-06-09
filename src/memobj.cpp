#include "memobj.h"
#include "types.h"
#include "common.h"
#include <node_buffer.h>
#include "nanextension.h"

using namespace node;

namespace opencl {

// /* Memory Object APIs */
// extern CL_API_ENTRY cl_mem CL_API_CALL
// clCreateBuffer(cl_context   /* context */,
//                cl_mem_flags /* flags */,
//                size_t       /* size */,
//                void *       /* host_ptr */,
//                cl_int *     /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateBuffer) {
  Nan::HandleScope scope;
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 1
  cl_mem_flags flags = Nan::To<uint32_t>(info[1]).FromJust();

  // Arg 2
  size_t size = Nan::To<uint32_t>(info[2]).FromJust();

  // Arg 3
  void *host_ptr = NULL;
  if(ARG_EXISTS(3)) {
    size_t len=0;
    getPtrAndLen(info[3], host_ptr, len);
    // std::cout<<"[CreateBuffer] host_ptr 0x"<<std::hex<<host_ptr<<std::dec<<std::endl;
    // for(int i=0;i<len;i++)
    //   std::cout<<((uint8_t*)host_ptr)[i]<<", ";
    // std::cout<<std::endl;
    if(!host_ptr || !len)
      return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  cl_int ret=CL_SUCCESS;
  cl_mem mem = ::clCreateBuffer(context->getRaw(), flags, size, host_ptr, &ret);
  CHECK_ERR(ret);

  // if(host_ptr) {
  //   NoCLAvoidGC* user_data = new NoCLAvoidGC(info[3].As<Object>());
  //   clSetMemObjectDestructorCallback(mem,notifyFreeClMemObj,user_data);
  // }

  info.GetReturnValue().Set(NOCL_WRAP(NoCLMem, mem));
}

// extern CL_API_ENTRY cl_mem CL_API_CALL
// clCreateSubBuffer(cl_mem                   /* buffer */,
//                   cl_mem_flags             /* flags */,
//                   cl_buffer_create_type    /* buffer_create_type */,
//                   const void *             /* buffer_create_info */,
//                   cl_int *                 /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;
NAN_METHOD(CreateSubBuffer) {
  Nan::HandleScope scope;
  REQ_ARGS(4);

  // Arg 0
  NOCL_UNWRAP(buffer, NoCLMem, info[0]);

  // Arg 1
  cl_mem_flags flags = Nan::To<uint32_t>(info[1]).FromJust();

  // Arg 2
  cl_buffer_create_type buffer_create_type = Nan::To<uint32_t>(info[2]).FromJust();

  if(buffer_create_type==CL_BUFFER_CREATE_TYPE_REGION) {
    Local<Object> obj = Nan::To<Object>(info[3]).ToLocalChecked();
    cl_buffer_region buffer_create_info;
    buffer_create_info.origin = Nan::To<uint32_t>(Nan::Get(obj, JS_STR("origin")).ToLocalChecked()).FromJust();
    buffer_create_info.size = Nan::To<uint32_t>(Nan::Get(obj, JS_STR("size")).ToLocalChecked()).FromJust();

    cl_int ret=CL_SUCCESS;
    cl_mem mem = ::clCreateSubBuffer(buffer->getRaw(), flags, buffer_create_type, &buffer_create_info, &ret);
    CHECK_ERR(ret);

    info.GetReturnValue().Set(NOCL_WRAP(NoCLMem, mem));
    return;
  }

  CHECK_ERR(CL_INVALID_VALUE);
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}
#ifdef CL_VERSION_1_2
// extern CL_API_ENTRY cl_mem CL_API_CALL
// clCreateImage(cl_context              /* context */,
//               cl_mem_flags            /* flags */,
//               const cl_image_format * /* image_format */,
//               const cl_image_desc *   /* image_desc */,
//               void *                  /* host_ptr */,
//               cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(CreateImage) {
  Nan::HandleScope scope;
  REQ_ARGS(4);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 1
  cl_mem_flags flags = Nan::To<uint32_t>(info[1]).FromJust();

  // Arg 2
  Local<Object> obj = Nan::To<Object>(info[2]).ToLocalChecked();
  cl_image_format image_format;
  image_format.image_channel_order = Nan::Get(obj, JS_STR("channel_order")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("channel_order")).ToLocalChecked()).FromJust();
  image_format.image_channel_data_type = Nan::Get(obj, JS_STR("channel_data_type")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("channel_data_type")).ToLocalChecked()).FromJust();

  // Arg 3
  obj = Nan::To<Object>(info[3]).ToLocalChecked();
  cl_image_desc desc;
  memset(&desc,0,sizeof(cl_image_desc));


  desc.image_type = Nan::Get(obj, JS_STR("type")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("type")).ToLocalChecked()).FromJust();
  desc.image_width = Nan::Get(obj, JS_STR("width")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("width")).ToLocalChecked()).FromJust();
  desc.image_height = Nan::Get(obj, JS_STR("height")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("height")).ToLocalChecked()).FromJust();
  desc.image_depth = Nan::Get(obj, JS_STR("depth")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("depth")).ToLocalChecked()).FromJust();
  desc.image_array_size = Nan::Get(obj, JS_STR("array_size")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("array_size")).ToLocalChecked()).FromJust();
  desc.image_row_pitch = Nan::Get(obj, JS_STR("row_pitch")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("row_pitch")).ToLocalChecked()).FromJust();
  desc.image_slice_pitch = Nan::Get(obj, JS_STR("slice_pitch")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("slice_pitch")).ToLocalChecked()).FromJust();
  Local<Value> buffer_value = Nan::Get(obj, JS_STR("buffer")).ToLocalChecked();
  if (buffer_value->IsObject()) {
    NOCL_UNWRAP(buffer, NoCLMem, buffer_value);
    desc.buffer = buffer->getRaw();
  }

  void *host_ptr = NULL;

  // Arg 4
  if(ARG_EXISTS(4)) {
    size_t len = 0;
    getPtrAndLen(info[4], host_ptr, len);

    if(!host_ptr || !len)
      return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  cl_int ret=CL_SUCCESS;
  cl_mem mem = ::clCreateImage(context->getRaw(), flags, &image_format, &desc, host_ptr, &ret);
  CHECK_ERR(ret);

  // if(host_ptr) {
  //   NoCLAvoidGC* user_data = new NoCLAvoidGC(info[3].As<Object>());
  //   clSetMemObjectDestructorCallback(mem,notifyFreeClMemObj,user_data);
  // }

  info.GetReturnValue().Set(NOCL_WRAP(NoCLMem, mem));
}
#endif
#ifdef CL_VERSION_1_1
//TODO createImage2D/3D from 1.1 spec
/*
 *  cl_mem clCreateImage2D (
 *      cl_context context,
        cl_mem_flags flags,
        const cl_image_format *image_format,
        size_t image_width,
        size_t image_height,
        size_t image_row_pitch,
        void *host_ptr,
        cl_int *errcode_ret)
 * */
NAN_METHOD(CreateImage2D) {
  Nan::HandleScope scope;
  REQ_ARGS(7);

  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 1
  cl_mem_flags flags = Nan::To<uint32_t>(info[1]).FromJust();

  //
  Local<Object> obj = Nan::To<Object>(info[2]).ToLocalChecked();
  cl_image_format image_format;
  image_format.image_channel_order = Nan::Get(obj, JS_STR("channel_order")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("channel_order")).ToLocalChecked()).FromJust();
  image_format.image_channel_data_type = Nan::Get(obj, JS_STR("channel_data_type")).ToLocalChecked()->IsUndefined() ? 0 : Nan::To<uint32_t>(Nan::Get(obj, JS_STR("channel_data_type")).ToLocalChecked()).FromJust();

  size_t image_width = Nan::To<uint32_t>(info[3]).FromJust();
  size_t image_height = Nan::To<uint32_t>(info[4]).FromJust();
  size_t image_row_pitch = Nan::To<uint32_t>(info[5]).FromJust();

  void *host_ptr = NULL;

  // Arg 4
  if(ARG_EXISTS(6)) {
    size_t len = 0;
    getPtrAndLen(info[6], host_ptr, len);

    if(!host_ptr || !len)
      return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  cl_int ret=CL_SUCCESS;
  cl_mem mem = ::clCreateImage2D(context->getRaw(), flags, &image_format, image_width,image_height,image_row_pitch, host_ptr, &ret);
  CHECK_ERR(ret);

  // if(host_ptr) {
  //   NoCLAvoidGC* user_data = new NoCLAvoidGC(info[3].As<Object>());
  //   clSetMemObjectDestructorCallback(mem,notifyFreeClMemObj,user_data);
  // }

  info.GetReturnValue().Set(NOCL_WRAP(NoCLMem, mem));
}
#endif
// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainMemObject(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainMemObject) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(mem, NoCLMem, info[0]);

  cl_int ret=mem->acquire();

  CHECK_ERR(ret);
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseMemObject(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseMemObject) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(mem, NoCLMem, info[0]);
  cl_int ret=mem->release();

  CHECK_ERR(ret);
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetSupportedImageFormats(cl_context           /* context */,
//                            cl_mem_flags         /* flags */,
//                            cl_mem_object_type   /* image_type */,
//                            cl_uint              /* num_entries */,
//                            cl_image_format *    /* image_formats */,
//                            cl_uint *            /* num_image_formats */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetSupportedImageFormats) {
  Nan::HandleScope scope;
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 1
  cl_mem_flags flags = Nan::To<uint32_t>(info[1]).FromJust();

  // Arg 2
  cl_mem_object_type image_type = Nan::To<uint32_t>(info[2]).FromJust();

  cl_uint numEntries=0;
  CHECK_ERR(::clGetSupportedImageFormats(context->getRaw(), flags, image_type, 0, NULL, &numEntries));

  unique_ptr<cl_image_format[]> image_formats(new cl_image_format[numEntries]);
  CHECK_ERR(::clGetSupportedImageFormats(context->getRaw(), flags, image_type, numEntries, image_formats.get(), NULL));

  Local<Array> imageFormats = Nan::New<Array>();
  for (uint32_t i=0; i<numEntries; i++) {
    Local<Object> format = Nan::New<Object>();
    Nan::Set(format, JS_STR("channel_order"), JS_INT(image_formats[i].image_channel_order));
    Nan::Set(format, JS_STR("channel_data_type"), JS_INT(image_formats[i].image_channel_data_type));
    Nan::Set(imageFormats, i, format);
  }

  info.GetReturnValue().Set(imageFormats);
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetMemObjectInfo(cl_mem           /* memobj */,
//                    cl_mem_info      /* param_name */,
//                    size_t           /* param_value_size */,
//                    void *           /* param_value */,
//                    size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetMemObjectInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(mem, NoCLMem, info[0]);

  // Arg 1
  cl_mem_info param_name = Nan::To<uint32_t>(info[1]).FromJust();

  switch(param_name) {
    case CL_MEM_TYPE: {
      cl_mem_object_type val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_mem_object_type), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_MEM_FLAGS: {
      cl_mem_flags val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_mem_flags), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_MEM_SIZE:
    case CL_MEM_OFFSET:
    {
      size_t val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(size_t), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_MEM_MAP_COUNT:
    case CL_MEM_REFERENCE_COUNT:
    {
      cl_uint val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_MEM_HOST_PTR: {
      void* val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(void*), &val, NULL))

      //info.GetReturnValue().Set(NOCL_WRAP(NoCLMappedPtr, val));
      return;
    }
    case CL_MEM_CONTEXT: {
      cl_context val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_context), &val, NULL))
      CHECK_ERR(::clRetainContext(val))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLContext, val));
      return;
    }
    case CL_MEM_ASSOCIATED_MEMOBJECT: {
      cl_mem val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_mem), &val, NULL))
      CHECK_ERR(::clRetainMemObject(val))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLMem, val));
      return;
    }
  }
  return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(CL_INVALID_VALUE)));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetImageInfo(cl_mem           /* image */,
//                cl_image_info    /* param_name */,
//                size_t           /* param_value_size */,
//                void *           /* param_value */,
//                size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetImageInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(mem, NoCLMem, info[0]);

  // Arg 1
  cl_image_info param_name = Nan::To<uint32_t>(info[1]).FromJust();

  switch(param_name) {
    case CL_IMAGE_FORMAT: {
      cl_image_format val;
      CHECK_ERR(::clGetImageInfo(mem->getRaw(),param_name,sizeof(cl_image_format), &val, NULL))
      Local<Array> arr=Nan::New<Array>(2);
      Nan::Set(arr, JS_STR("channel_order"), JS_INT(val.image_channel_order));
      Nan::Set(arr, JS_STR("channel_data_type"), JS_INT(val.image_channel_data_type));
      info.GetReturnValue().Set(arr);
      return;
    }
    case CL_IMAGE_ELEMENT_SIZE:
    case CL_IMAGE_ROW_PITCH:
    case CL_IMAGE_SLICE_PITCH:
    case CL_IMAGE_WIDTH:
    case CL_IMAGE_HEIGHT:
    case CL_IMAGE_DEPTH:
#ifdef CL_VERSION_1_2
    case CL_IMAGE_ARRAY_SIZE:
#endif
    {
      size_t val;
      CHECK_ERR(::clGetImageInfo(mem->getRaw(),param_name,sizeof(size_t), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
#ifdef CL_VERSION_1_2
    case CL_IMAGE_BUFFER: {
      cl_mem val;
      CHECK_ERR(::clGetImageInfo(mem->getRaw(),param_name,sizeof(cl_mem), &val, NULL))
      CHECK_ERR(::clRetainMemObject(val))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLMem, val));
      return;
    }
    case CL_IMAGE_NUM_MIP_LEVELS:
    case CL_IMAGE_NUM_SAMPLES:
    {
      cl_uint val;
      CHECK_ERR(::clGetImageInfo(mem->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
#endif
  }

  return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(CL_INVALID_VALUE)));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetMemObjectDestructorCallback(  cl_mem /* memobj */,
//                                     void (CL_CALLBACK * /*pfn_notify*/)( cl_mem /* memobj */, void* /*user_data*/),
//                                     void * /*user_data */ )             CL_API_SUFFIX__VERSION_1_1;

namespace MemObj {
NAN_MODULE_INIT(init)
{
  Nan::SetMethod(target, "createBuffer", CreateBuffer);
  Nan::SetMethod(target, "createSubBuffer", CreateSubBuffer);
#ifdef CL_VERSION_1_2
  Nan::SetMethod(target, "createImage", CreateImage);
#endif
#ifdef CL_VERSION_1_1
  Nan::SetMethod(target, "createImage2D", CreateImage2D);
#endif
  Nan::SetMethod(target, "retainMemObject", RetainMemObject);
  Nan::SetMethod(target, "releaseMemObject", ReleaseMemObject);
  Nan::SetMethod(target, "getSupportedImageFormats", GetSupportedImageFormats);
  Nan::SetMethod(target, "getMemObjectInfo", GetMemObjectInfo);
  Nan::SetMethod(target, "getImageInfo", GetImageInfo);
}
} // namespace MemObj

} // namespace opencl
