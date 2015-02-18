#include "context.h"
#include <vector>

using namespace std;

namespace opencl {

// /* Context APIs  */
// extern CL_API_ENTRY cl_context CL_API_CALL
// clCreateContext(const cl_context_properties * /* properties */,
//                 cl_uint                 /* num_devices */,
//                 const cl_device_id *    /* devices */,
//                 void (CL_CALLBACK * /* pfn_notify */)(const char *, const void *, size_t, void *),
//                 void *                  /* user_data */,
//                 cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateContext) {
  NanEscapableScope();
  REQ_ARGS(4)
  Local<Array> properties;
  Local<Array> devices;
  Local<Function> callback;
  Local<Function> err_cb;
  vector<cl_context_properties> cl_properties;
  vector<cl_device_id> cl_devices;
  cl_context ctx=NULL;
  int err=CL_SUCCESS;

  // Arg 1 -- properties
  if(ARG_EXISTS(0)) {
    REQ_ARRAY_ARG(0, properties);
    for (uint32_t i=0; i < properties->Length(); i++) {
      cl_uint prop_id = properties->Get(i)->Uint32Value();
      cl_properties.push_back(prop_id);
      if(prop_id == CL_CONTEXT_PLATFORM) {
        NOCL_UNWRAP(platform, NoCLPlatformId, properties->Get(++i));
        cl_properties.push_back((cl_context_properties) platform->getRaw());
      }
    }
    cl_properties.push_back(0);
  }

  // Arg 2 -- devices
  if(ARG_EXISTS(1)) {
    REQ_ARRAY_ARG(1, devices);
    for (uint32_t i=0; i<devices->Length(); i++) {
      NOCL_UNWRAP(device, NoCLDeviceId, devices->Get(i));
      cl_devices.push_back(device->getRaw());
        // printf("Adding device %p\n",device);
    }
  }

  // Arg 3 -- Callback
  if(ARG_EXISTS(2)) {
    callback = Local<Function>::Cast(args[2]);
  }

  // Arg 4 -- Error callback
  if(ARG_EXISTS(3)) {
    err_cb = Local<Function>::Cast(args[3]);
  }


  ctx = ::clCreateContext(&cl_properties.front(),
                        (int) cl_devices.size(), &cl_devices.front(),
                         NULL, NULL, // TODO callback support
                         &err);

  CHECK_ERR(err);
  Local<Object> obj = NoCLWrapCLObject<NoCLContext>(new NoCLContext(ctx));

  NanReturnValue(obj);
}

// extern CL_API_ENTRY cl_context CL_API_CALL
// clCreateContextFromType(const cl_context_properties * /* properties */,
//                         cl_device_type          /* device_type */,
//                         void (CL_CALLBACK *     /* pfn_notify*/ )(const char *, const void *, size_t, void *),
//                         void *                  /* user_data */,
//                         cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateContextFromType) {
  NanEscapableScope();
  REQ_ARGS(4)

  Local<Array> properties;
  vector<cl_context_properties> cl_properties;
  Local<Function> callback;
  Local<Function> err_cb;

  if(!args[0]->IsNull() && !args[0]->IsUndefined()) {
    REQ_ARRAY_ARG(0, properties);
    for (uint32_t i=0; i < properties->Length(); i++) {
      cl_uint prop_id = properties->Get(i)->Uint32Value();
      cl_properties.push_back(prop_id);
      if(prop_id == CL_CONTEXT_PLATFORM) {
        NOCL_UNWRAP(platform, NoCLPlatformId, properties->Get(++i));
        cl_properties.push_back((cl_context_properties) platform->getRaw());
        // printf("Adding platform %p\n",platform);
      }
    }
    cl_properties.push_back(0);
  }

  cl_device_type device_type=args[1]->Uint32Value();

  if(!args[2]->IsNull() && !args[2]->IsUndefined()) {
      callback = Local<Function>::Cast(args[2]);
  }
  if(!args[3]->IsNull() && !args[3]->IsUndefined()) {
      err_cb = Local<Function>::Cast(args[3]);
  }

  int err=CL_SUCCESS;
  cl_context ctx = ::clCreateContextFromType(&cl_properties.front(),
                        device_type,
                        NULL, NULL, // TODO callback support
                        &err);
  CHECK_ERR(err);

  NanReturnValue(NoCLWrapCLObject<NoCLContext>(new NoCLContext(ctx)));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainContext(cl_context /* context */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainContext) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(context, NoCLContext, args[0]);
  cl_int count=clRetainContext(context->getRaw());

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseContext(cl_context /* context */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseContext) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(context, NoCLContext, args[0]);
  cl_int count=clReleaseContext(context->getRaw());

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetContextInfo(cl_context         /* context */,
//                  cl_context_info    /* param_name */,
//                  size_t             /* param_value_size */,
//                  void *             /* param_value */,
//                  size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetContextInfo) {
  NanScope();

  NOCL_UNWRAP(context, NoCLContext, args[0]);
  cl_context_info param_name = args[1]->Uint32Value();

  switch (param_name) {
  case CL_CONTEXT_REFERENCE_COUNT:
  case CL_CONTEXT_NUM_DEVICES: {
    cl_uint param_value=0;
    CHECK_ERR(::clGetContextInfo(context->getRaw(),param_name,sizeof(cl_uint), &param_value, NULL));
    NanReturnValue(JS_INT(param_value));
  }
  case CL_CONTEXT_DEVICES: {
    size_t n=0;
    CHECK_ERR(::clGetContextInfo(context->getRaw(),param_name,0,NULL, &n));
    n /= sizeof(cl_device_id);

    unique_ptr<cl_device_id[]> devices(new cl_device_id[n]);
    CHECK_ERR(::clGetContextInfo(context->getRaw(),param_name,sizeof(cl_device_id)*n, devices.get(), NULL));

    Local<Array> arr = NanNew<Array>((int)n);
    for(uint32_t i=0;i<n;i++) {
      arr->Set(i, NOCL_WRAP(NoCLDeviceId, devices[i]));
    }
    NanReturnValue(arr);
  }
  case CL_CONTEXT_PROPERTIES: {
    size_t n=0;
    CHECK_ERR(::clGetContextInfo(context->getRaw(),param_name,0,NULL, &n));
    unique_ptr<cl_context_properties[]> ctx(new cl_context_properties[n]);
    CHECK_ERR(::clGetContextInfo(context->getRaw(),param_name,sizeof(cl_context_properties)*n, ctx.get(), NULL));

    Local<Array> arr = NanNew<Array>((int)n);
    for(uint32_t i=0;i<n;i++) {
      arr->Set(i,JS_INT((int32_t)ctx[i]));
    }

    NanReturnValue(arr);
  }
  default: {
    THROW_ERR(CL_INVALID_VALUE);
  }
  }

  NanReturnUndefined();
}

// // Deprecated OpenCL 1.1 APIs
// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL
// clCreateImage2D(cl_context              /* context */,
//                 cl_mem_flags            /* flags */,
//                 const cl_image_format * /* image_format */,
//                 size_t                  /* image_width */,
//                 size_t                  /* image_height */,
//                 size_t                  /* image_row_pitch */,
//                 void *                  /* host_ptr */,
//                 cl_int *                /* errcode_ret */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL
// clCreateImage3D(cl_context              /* context */,
//                 cl_mem_flags            /* flags */,
//                 const cl_image_format * /* image_format */,
//                 size_t                  /* image_width */,
//                 size_t                  /* image_height */,
//                 size_t                  /* image_depth */,
//                 size_t                  /* image_row_pitch */,
//                 size_t                  /* image_slice_pitch */,
//                 void *                  /* host_ptr */,
//                 cl_int *                /* errcode_ret */) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

// extern CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
// clUnloadCompiler(void) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
namespace Context {
void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "createContext", CreateContext);
  NODE_SET_METHOD(exports, "createContextFromType", CreateContextFromType);
  NODE_SET_METHOD(exports, "retainContext", RetainContext);
  NODE_SET_METHOD(exports, "releaseContext", ReleaseContext);
  NODE_SET_METHOD(exports, "getContextInfo", GetContextInfo);
}
} // namespace Context

} // namespace opencl
