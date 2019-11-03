#include "platform.h"
#include "types.h"

namespace opencl {

/* Platform API */
// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetPlatformIDs(cl_uint          /* num_entries */,
//                  cl_platform_id * /* platforms */,
//                  cl_uint *        /* num_platforms */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetPlatformIDs) {
  Nan::HandleScope scope;

  cl_uint num_entries = 0;
  CHECK_ERR(::clGetPlatformIDs(0, NULL, &num_entries));

  unique_ptr<cl_platform_id[]> platforms(new cl_platform_id[num_entries]);
  CHECK_ERR(::clGetPlatformIDs(num_entries, platforms.get(), NULL));

  Local<Array> platformArray = Nan::New<Array>(num_entries);
  for (uint32_t i=0; i<num_entries; i++) {
    platformArray->Set(i, NOCL_WRAP(NoCLPlatformId, platforms[i]));
  }

  info.GetReturnValue().Set(platformArray);
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetPlatformInfo(cl_platform_id   /* platform */,
//                   cl_platform_info /* param_name */,
//                   size_t           /* param_value_size */,
//                   void *           /* param_value */,
//                   size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetPlatformInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  NOCL_UNWRAP(platform_id, NoCLPlatformId, info[0]);
  cl_platform_info param_name = Nan::To<uint32_t>(info[1]).FromJust();

  char param_value[1024];
  size_t param_value_size_ret=0;

  CHECK_ERR(::clGetPlatformInfo(platform_id->getRaw(), param_name, 1024, param_value, &param_value_size_ret));

  // NOTE: Adjust length because API returns NULL terminated string
  info.GetReturnValue().Set(JS_STR(param_value,(int)param_value_size_ret - 1));
}

namespace Platform {
NAN_MODULE_INIT(init)
{
  Nan::SetMethod(target, "getPlatformIDs", GetPlatformIDs);
  Nan::SetMethod(target, "getPlatformInfo", GetPlatformInfo);
}
} // namespace Platform

} // namespace opencl
