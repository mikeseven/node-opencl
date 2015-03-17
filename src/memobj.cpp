#include "memobj.h"
#include "types.h"
#include "common.h"
#include <node_buffer.h>
#include "nanextension.h"

using namespace node;

namespace opencl {


class NoCLAvoidGC:public NanAsyncLaunch {
 public:
   NoCLAvoidGC(const v8::Local<v8::Object> &array):NanAsyncLaunch(nullptr){
       NanScope();
       v8::Local<v8::Object> obj = NanNew<v8::Object>();
       NanAssignPersistent(persistentHandle, obj);
       v8::Local<v8::Object>  handle = NanNew(persistentHandle);
       handle->Set(kIndex, array);
   }

   void Execute() {
       return;
   }

 private:

};

void CL_CALLBACK notifyFreeClMemObj (cl_mem mem,void* user_data) {
    NoCLAvoidGC* asyncCB = static_cast<NoCLAvoidGC*>(user_data);
    if(asyncCB!=nullptr)
        asyncCB->FireAndForget();
}


// /* Memory Object APIs */
// extern CL_API_ENTRY cl_mem CL_API_CALL
// clCreateBuffer(cl_context   /* context */,
//                cl_mem_flags /* flags */,
//                size_t       /* size */,
//                void *       /* host_ptr */,
//                cl_int *     /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateBuffer) {
  NanScope();
  REQ_ARGS(4);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 1
  cl_mem_flags flags = args[1]->Uint32Value();

  // Arg 2
  size_t size = args[2]->Uint32Value();

  // Arg 3
  void *host_ptr = NULL;
  if(ARG_EXISTS(3)) {
    if(args[3]->IsArray()) {
      THROW_ERR(CL_INVALID_MEM_OBJECT);
    }
    if(args[3]->IsObject()) {
      Local<Object> obj=args[3]->ToObject();
      String::Utf8Value name(obj->GetConstructorName());
      if(strcmp("Buffer",*name))
        host_ptr=Buffer::Data(obj);
      else if(strcmp("Array",*name)) {
        // TypedArray
        if (!obj->HasIndexedPropertiesInExternalArrayData()) {
          THROW_ERR(CL_INVALID_MEM_OBJECT);
        }
        host_ptr=obj->GetIndexedPropertiesExternalArrayData();
      } else {
        THROW_ERR(CL_INVALID_MEM_OBJECT);
      }
    }
    else
    THROW_ERR(CL_INVALID_MEM_OBJECT);
  }

  cl_int ret=CL_SUCCESS;
  cl_mem mem = ::clCreateBuffer(context->getRaw(), flags, size, host_ptr, &ret);
  CHECK_ERR(ret);

  if(host_ptr) {
    NoCLAvoidGC* user_data = new NoCLAvoidGC(args[3].As<Object>());
    clSetMemObjectDestructorCallback(mem,notifyFreeClMemObj,user_data);
  }

  NanReturnValue(NOCL_WRAP(NoCLMem, mem));
}

// extern CL_API_ENTRY cl_mem CL_API_CALL
// clCreateSubBuffer(cl_mem                   /* buffer */,
//                   cl_mem_flags             /* flags */,
//                   cl_buffer_create_type    /* buffer_create_type */,
//                   const void *             /* buffer_create_info */,
//                   cl_int *                 /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;
NAN_METHOD(CreateSubBuffer) {
  NanScope();
  REQ_ARGS(4);

  // Arg 0
  NOCL_UNWRAP(buffer, NoCLMem, args[0]);

  // Arg 1
  cl_mem_flags flags = args[1]->Uint32Value();

  // Arg 2
  cl_buffer_create_type buffer_create_type = args[2]->Uint32Value();

  if(buffer_create_type==CL_BUFFER_CREATE_TYPE_REGION) {
    Local<Object> obj = args[3]->ToObject();
    cl_buffer_region buffer_create_info;
    buffer_create_info.origin = obj->Get(JS_STR("origin"))->Uint32Value();
    buffer_create_info.size = obj->Get(JS_STR("size"))->Uint32Value();

    cl_int ret=CL_SUCCESS;
    cl_mem mem = ::clCreateSubBuffer(buffer->getRaw(), flags, buffer_create_type, &buffer_create_info, &ret);
    CHECK_ERR(ret);

    NanReturnValue(NOCL_WRAP(NoCLMem, mem));
  }

  CHECK_ERR(CL_INVALID_VALUE);
  NanReturnValue(JS_INT(CL_SUCCESS));
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
  NanScope();
  REQ_ARGS(5);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 1
  cl_mem_flags flags = args[1]->Uint32Value();

  // Arg 2
  Local<Object> obj = args[2]->ToObject();
  cl_image_format image_format;
  image_format.image_channel_order = obj->Get(JS_STR("channel_order"))->IsUndefined() ? 0 : obj->Get(JS_STR("channel_order"))->Uint32Value();
  image_format.image_channel_data_type = obj->Get(JS_STR("channel_data_type"))->IsUndefined() ? 0 : obj->Get(JS_STR("channel_data_type"))->Uint32Value();

  // Arg 3
  obj = args[3]->ToObject();
  cl_image_desc desc;
  memset(&desc,0,sizeof(cl_image_desc));


  desc.image_type = obj->Get(JS_STR("type"))->IsUndefined() ? 0 : obj->Get(JS_STR("type"))->Uint32Value();
  desc.image_width = obj->Get(JS_STR("width"))->IsUndefined() ? 0 : obj->Get(JS_STR("width"))->Uint32Value();
  desc.image_height = obj->Get(JS_STR("height"))->IsUndefined() ? 0 : obj->Get(JS_STR("height"))->Uint32Value();
  desc.image_depth = obj->Get(JS_STR("depth"))->IsUndefined() ? 0 : obj->Get(JS_STR("depth"))->Uint32Value();
  desc.image_array_size = obj->Get(JS_STR("array_size"))->IsUndefined() ? 0 : obj->Get(JS_STR("array_size"))->Uint32Value();
  desc.image_row_pitch = obj->Get(JS_STR("row_pitch"))->IsUndefined() ? 0 : obj->Get(JS_STR("row_pitch"))->Uint32Value();
  desc.image_slice_pitch = obj->Get(JS_STR("slice_pitch"))->IsUndefined() ? 0 : obj->Get(JS_STR("slice_pitch"))->Uint32Value();;
  // desc.num_mip_levels = obj->Get(JS_STR("channel_order"))->Uint32Value();;
  // desc.num_samples = obj->Get(JS_STR("channel_order"))->Uint32Value();;
  // desc.buffer;

  void *host_ptr = NULL;

  // Arg 4
  if(ARG_EXISTS(4)) {
    if(args[4]->IsArray()) {
      // JS Array
      Local<Array> arr=Local<Array>::Cast(args[4]);
      host_ptr=arr->GetIndexedPropertiesExternalArrayData();
    }
    else if(args[4]->IsObject()) {
      Local<Object> obj=args[4]->ToObject();
      String::Utf8Value name(obj->GetConstructorName());
      if(!strcmp("Buffer",*name))
        host_ptr=Buffer::Data(obj);
      else {
        // TypedArray
        if(!obj->HasIndexedPropertiesInExternalArrayData()) {
          THROW_ERR(CL_INVALID_MEM_OBJECT)
        }
        host_ptr=obj->GetIndexedPropertiesExternalArrayData();
        // printf("external array data type %d\n",obj->GetIndexedPropertiesExternalArrayDataType());
      }
    }
    else
      NanThrowError("Invalid memory object");
  }

  cl_int ret=CL_SUCCESS;
  cl_mem mem = ::clCreateImage(context->getRaw(), flags, &image_format, &desc, host_ptr, &ret);
  CHECK_ERR(ret);

  if(host_ptr) {
    NoCLAvoidGC* user_data = new NoCLAvoidGC(args[3].As<Object>());
    clSetMemObjectDestructorCallback(mem,notifyFreeClMemObj,user_data);
  }

  NanReturnValue(NOCL_WRAP(NoCLMem, mem));
}
#elif CL_VERSION_1_1
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
  NanScope();
  REQ_ARGS(7);

  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 1
  cl_mem_flags flags = args[1]->Uint32Value();

  //
  Local<Object> obj = args[2]->ToObject();
  cl_image_format image_format;
  image_format.image_channel_order = obj->Get(JS_STR("channel_order"))->IsUndefined() ? 0 : obj->Get(JS_STR("channel_order"))->Uint32Value();
  image_format.image_channel_data_type = obj->Get(JS_STR("channel_data_type"))->IsUndefined() ? 0 : obj->Get(JS_STR("channel_data_type"))->Uint32Value();

  size_t image_width = args[3]->Uint32Value();
  size_t image_height = args[4]->Uint32Value();
  size_t image_row_pitch = args[5]->Uint32Value();

  void *host_ptr = NULL;

  // Arg 4
  if(ARG_EXISTS(6)) {
    if(args[6]->IsArray()) {
      // JS Array
      Local<Array> arr=Local<Array>::Cast(args[6]);
      host_ptr=arr->GetIndexedPropertiesExternalArrayData();
    }
    else if(args[6]->IsObject()) {
      Local<Object> obj=args[6]->ToObject();
      String::Utf8Value name(obj->GetConstructorName());
      if(!strcmp("Buffer",*name))
        host_ptr=Buffer::Data(obj);
      else {
        // TypedArray
        if(!obj->HasIndexedPropertiesInExternalArrayData()) {
          THROW_ERR(CL_INVALID_MEM_OBJECT)
        }
        host_ptr=obj->GetIndexedPropertiesExternalArrayData();
      }
    }
    else
      NanThrowError("Invalid memory object");
  }

  cl_int ret=CL_SUCCESS;
  cl_mem mem = ::clCreateImage2D(context->getRaw(), flags, &image_format, image_width,image_height,image_row_pitch, host_ptr, &ret);
  CHECK_ERR(ret);

  if(host_ptr) {
    NoCLAvoidGC* user_data = new NoCLAvoidGC(args[3].As<Object>());
    clSetMemObjectDestructorCallback(mem,notifyFreeClMemObj,user_data);
  }

  NanReturnValue(NOCL_WRAP(NoCLMem, mem));
}
#endif
// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainMemObject(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainMemObject) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(mem, NoCLMem, args[0]);

  cl_int ret=mem->acquire();

  CHECK_ERR(ret);
  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseMemObject(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseMemObject) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(mem, NoCLMem, args[0]);
  cl_int ret=mem->release();

  CHECK_ERR(ret);
  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetSupportedImageFormats(cl_context           /* context */,
//                            cl_mem_flags         /* flags */,
//                            cl_mem_object_type   /* image_type */,
//                            cl_uint              /* num_entries */,
//                            cl_image_format *    /* image_formats */,
//                            cl_uint *            /* num_image_formats */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetSupportedImageFormats) {
  NanScope();
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 1
  cl_mem_flags flags = args[1]->Uint32Value();

  // Arg 2
  cl_mem_object_type image_type = args[2]->Uint32Value();

  cl_uint numEntries=0;
  CHECK_ERR(::clGetSupportedImageFormats(context->getRaw(), flags, image_type, 0, NULL, &numEntries));

  unique_ptr<cl_image_format[]> image_formats(new cl_image_format[numEntries]);
  CHECK_ERR(::clGetSupportedImageFormats(context->getRaw(), flags, image_type, numEntries, image_formats.get(), NULL));

  Local<Array> imageFormats = NanNew<Array>();
  for (uint32_t i=0; i<numEntries; i++) {
    Local<Object> format = NanNew<Object>();
    format->Set(JS_STR("channel_order"), JS_INT(image_formats[i].image_channel_order));
    format->Set(JS_STR("channel_data_type"), JS_INT(image_formats[i].image_channel_data_type));
    imageFormats->Set(i, format);
  }

  NanReturnValue(imageFormats);
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetMemObjectInfo(cl_mem           /* memobj */,
//                    cl_mem_info      /* param_name */,
//                    size_t           /* param_value_size */,
//                    void *           /* param_value */,
//                    size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetMemObjectInfo) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(mem, NoCLMem, args[0]);

  // Arg 1
  cl_mem_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_MEM_TYPE: {
      cl_mem_object_type val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_mem_object_type), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_MEM_FLAGS: {
      cl_mem_flags val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_mem_flags), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_MEM_SIZE:
    case CL_MEM_OFFSET:
    {
      size_t val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(size_t), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_MEM_MAP_COUNT:
    case CL_MEM_REFERENCE_COUNT:
    {
      cl_uint val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_MEM_HOST_PTR: {
      void* val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(void*), &val, NULL))

      //NanReturnValue(NOCL_WRAP(NoCLMappedPtr, val));
    }
    case CL_MEM_CONTEXT: {
      cl_context val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_context), &val, NULL))
      NanReturnValue(NOCL_WRAP(NoCLContext, val));
    }
    case CL_MEM_ASSOCIATED_MEMOBJECT: {
      cl_mem val;
      CHECK_ERR(::clGetMemObjectInfo(mem->getRaw(),param_name,sizeof(cl_mem), &val, NULL))
      NanReturnValue(NOCL_WRAP(NoCLMem, val));
    }
  }
  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetImageInfo(cl_mem           /* image */,
//                cl_image_info    /* param_name */,
//                size_t           /* param_value_size */,
//                void *           /* param_value */,
//                size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetImageInfo) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(mem, NoCLMem, args[0]);

  // Arg 1
  cl_image_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_IMAGE_FORMAT: {
      cl_image_format val;
      CHECK_ERR(::clGetImageInfo(mem->getRaw(),param_name,sizeof(cl_image_format), &val, NULL))
      Local<Array> arr=NanNew<Array>(2);
      arr->Set(JS_STR("channel_order"), JS_INT(val.image_channel_order));
      arr->Set(JS_STR("channel_data_type"), JS_INT(val.image_channel_data_type));
      NanReturnValue(arr);
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
      NanReturnValue(JS_INT(val));
    }
#ifdef CL_VERSION_1_2
    case CL_IMAGE_BUFFER: {
      cl_mem val;
      CHECK_ERR(::clGetImageInfo(mem->getRaw(),param_name,sizeof(cl_mem), &val, NULL))
      NanReturnValue(NOCL_WRAP(NoCLMem, val));
    }
    case CL_IMAGE_NUM_MIP_LEVELS:
    case CL_IMAGE_NUM_SAMPLES:
    {
      cl_uint val;
      CHECK_ERR(::clGetImageInfo(mem->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
#endif
  }

  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetMemObjectDestructorCallback(  cl_mem /* memobj */,
//                                     void (CL_CALLBACK * /*pfn_notify*/)( cl_mem /* memobj */, void* /*user_data*/),
//                                     void * /*user_data */ )             CL_API_SUFFIX__VERSION_1_1;

namespace MemObj {
void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "createBuffer", CreateBuffer);
  NODE_SET_METHOD(exports, "createSubBuffer", CreateSubBuffer);
#ifdef CL_VERSION_1_2
  NODE_SET_METHOD(exports, "createImage", CreateImage);
#elif CL_VERSION_1_1
  NODE_SET_METHOD(exports, "createImage2D", CreateImage2D);
#endif
  NODE_SET_METHOD(exports, "retainMemObject", RetainMemObject);
  NODE_SET_METHOD(exports, "releaseMemObject", ReleaseMemObject);
  NODE_SET_METHOD(exports, "getSupportedImageFormats", GetSupportedImageFormats);
  NODE_SET_METHOD(exports, "getMemObjectInfo", GetMemObjectInfo);
  NODE_SET_METHOD(exports, "getImageInfo", GetImageInfo);
}
} // namespace MemObj

} // namespace opencl
