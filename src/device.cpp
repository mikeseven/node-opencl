#include "device.h"
#include "types.h"

namespace opencl {

/* Device APIs */
// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetDeviceIDs(cl_platform_id   /* platform */,
//                cl_device_type   /* device_type */,
//                cl_uint          /* num_entries */,
//                cl_device_id *   /* devices */,
//                cl_uint *        /* num_devices */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetDeviceIDs) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(platform_id, NoCLPlatformId, args[0]);

  cl_device_type type = CL_DEVICE_TYPE_ALL;
  if(!args[1]->IsUndefined() && !args[1]->IsNull())
    type=args[1]->Uint32Value();

  cl_uint n = 0;
  CHECK_ERR(::clGetDeviceIDs(platform_id->getRaw(), type, 0, NULL, &n));

  unique_ptr<cl_device_id[]> devices(new cl_device_id[n]);
  CHECK_ERR(::clGetDeviceIDs(platform_id->getRaw(), type, n, devices.get(), NULL));

  Local<Array> deviceArray = NanNew<Array>(n);
  for (uint32_t i=0; i<n; i++) {
    deviceArray->Set(i, NOCL_WRAP(NoCLDeviceId, devices[i]));
  }

  NanReturnValue(deviceArray);
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetDeviceInfo(cl_device_id    /* device */,
//                 cl_device_info  /* param_name */,
//                 size_t          /* param_value_size */,
//                 void *          /* param_value */,
//                 size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetDeviceInfo) {
  NanScope();
  REQ_ARGS(2);

  NOCL_UNWRAP(deviceId, NoCLDeviceId, args[0]);

  cl_device_id device_id = deviceId->getRaw();
  cl_device_info param_name = args[1]->Uint32Value();

  switch (param_name) {
  case CL_DEVICE_NAME:
  case CL_DEVICE_VENDOR:
  case CL_DRIVER_VERSION:
  case CL_DEVICE_PROFILE:
  case CL_DEVICE_VERSION:
  case CL_DEVICE_OPENCL_C_VERSION:
  case CL_DEVICE_EXTENSIONS: {
    char param_value[1024];
    size_t param_value_size_ret=0;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(char)*1024, param_value, &param_value_size_ret));

    // NOTE: Adjust length because API returns NULL terminated string
    NanReturnValue(JS_STR(param_value,(int)param_value_size_ret - 1));
  }
  break;
  case CL_DEVICE_PLATFORM: {
    cl_platform_id param_value;

    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_platform_id), &param_value, NULL));

    if(param_value) {
      NanReturnValue(NOCL_WRAP(NoCLPlatformId, param_value));
    }
    NanReturnUndefined();
  }
  break;
  case CL_DEVICE_TYPE: {
    cl_device_type param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_type), &param_value, NULL));
    NanReturnValue(JS_INT(param_value));
  }
  break;
  case CL_DEVICE_LOCAL_MEM_TYPE: {
    cl_device_local_mem_type param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_local_mem_type), &param_value, NULL));
    NanReturnValue(JS_INT(param_value));
  }
  break;
  case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: {
    cl_device_mem_cache_type param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_mem_cache_type), &param_value, NULL));
    NanReturnValue(JS_INT(param_value));
  }
  break;
  case CL_DEVICE_EXECUTION_CAPABILITIES: {
    cl_device_exec_capabilities param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_exec_capabilities), &param_value, NULL));
    NanReturnValue(JS_INT((int)param_value));
  }
  break;
  case CL_DEVICE_QUEUE_PROPERTIES: {
    cl_command_queue_properties param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_command_queue_properties), &param_value, NULL));
    NanReturnValue(JS_INT((int)param_value));
  }
  break;
  case CL_DEVICE_HALF_FP_CONFIG:
  case CL_DEVICE_SINGLE_FP_CONFIG:
  case CL_DEVICE_DOUBLE_FP_CONFIG: {
    cl_device_fp_config param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_fp_config), &param_value, NULL));

    NanReturnValue(JS_INT((int)param_value));
  }
  break;
  case CL_DEVICE_MAX_WORK_ITEM_SIZES: {
    // get CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS param
    cl_uint max_work_item_dimensions;
    CHECK_ERR(::clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), &max_work_item_dimensions, NULL));

    // get CL_DEVICE_MAX_WORK_ITEM_SIZES array param
    unique_ptr<size_t[]> param_value(new size_t[max_work_item_dimensions]);
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, max_work_item_dimensions*sizeof(size_t), param_value.get(), NULL));

    Local<Array> arr = NanNew<Array>(max_work_item_dimensions);
    for(cl_uint i=0;i<max_work_item_dimensions;i++)
      arr->Set(i,JS_INT(param_value[i]));

    NanReturnValue(arr);
  }
  break;
  // cl_bool params
  case CL_DEVICE_AVAILABLE:
  case CL_DEVICE_COMPILER_AVAILABLE:
  case CL_DEVICE_ENDIAN_LITTLE:
  case CL_DEVICE_ERROR_CORRECTION_SUPPORT:
  case CL_DEVICE_HOST_UNIFIED_MEMORY:
  case CL_DEVICE_IMAGE_SUPPORT:
  {
    cl_bool param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_bool), &param_value, NULL));
    // keeping as Integer vs Boolean so comparisons with cl.TRUE/cl.FALSE work
    NanReturnValue(JS_BOOL((int)param_value));
  }
  break;
  // cl_uint params
  case CL_DEVICE_ADDRESS_BITS:
  case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
  case CL_DEVICE_MAX_CLOCK_FREQUENCY:
  case CL_DEVICE_MAX_COMPUTE_UNITS:
  case CL_DEVICE_MAX_CONSTANT_ARGS:
  case CL_DEVICE_MAX_READ_IMAGE_ARGS:
  case CL_DEVICE_MAX_SAMPLERS:
  case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
  case CL_DEVICE_MAX_WRITE_IMAGE_ARGS:
  case CL_DEVICE_MEM_BASE_ADDR_ALIGN:
  case CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:
  case CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR:
  case CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT:
  case CL_DEVICE_NATIVE_VECTOR_WIDTH_INT:
  case CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG:
  case CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT:
  case CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE:
  case CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF:
  case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
  case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
  case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:
  case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
  case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
  case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
  case CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF:
  case CL_DEVICE_VENDOR_ID:

  // OpenCL 1.2 constants
#ifdef CL_VERSION_1_2
  case CL_DEVICE_REFERENCE_COUNT:
  case CL_DEVICE_PARTITION_MAX_SUB_DEVICES:
#endif
  {
    cl_uint param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_uint), &param_value, NULL));
    NanReturnValue(JS_INT((int)param_value));
  }
  break;
  // cl_ulong params
  case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:
  case CL_DEVICE_GLOBAL_MEM_SIZE:
  case CL_DEVICE_LOCAL_MEM_SIZE:
  case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:
  case CL_DEVICE_MAX_MEM_ALLOC_SIZE: {
    cl_ulong param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_ulong), &param_value, NULL));

    // FIXME: handle uint64 somehow
    // JS only supports doubles, v8 has ints, CL params can be uint64
    // the memory params can certainly overflow uint32 size
    NanReturnValue(JS_INT((int)param_value));
  }
  break;
  // size_t params
  case CL_DEVICE_IMAGE2D_MAX_HEIGHT:
  case CL_DEVICE_IMAGE2D_MAX_WIDTH:
  case CL_DEVICE_IMAGE3D_MAX_DEPTH:
  case CL_DEVICE_IMAGE3D_MAX_HEIGHT:
  case CL_DEVICE_IMAGE3D_MAX_WIDTH:
  case CL_DEVICE_MAX_PARAMETER_SIZE:
  case CL_DEVICE_MAX_WORK_GROUP_SIZE:
  case CL_DEVICE_PROFILING_TIMER_RESOLUTION:

  // OpenCL 1.2 constants
#ifdef CL_VERSION_1_2
  case CL_DEVICE_IMAGE_MAX_BUFFER_SIZE:
  case CL_DEVICE_IMAGE_MAX_ARRAY_SIZE:
#endif
  {
    size_t param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(size_t), &param_value, NULL));

    // FIXME: handle 64 bit size_t somehow
    // assume for these params it will fit in an int
    NanReturnValue(NanNew<Integer>((int)param_value));
  }
  break;
  default: {
    THROW_ERR(CL_INVALID_VALUE);
  }
  }
  NanReturnUndefined();
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clCreateSubDevices(cl_device_id                         /* in_device */,
//                    const cl_device_partition_property * /* properties */,
//                    cl_uint                              /* num_devices */,
//                    cl_device_id *                       /* out_devices */,
//                    cl_uint *                            /* num_devices_ret */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(CreateSubDevices) {
  NanScope();
  // TODO
  NanReturnUndefined();

}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainDevice(cl_device_id /* device */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(RetainDevice) {
  NanScope();
  REQ_ARGS(1);

  // TODO make sure device_id is a sub-device
  NanReturnValue(JS_INT(0));

  // Local<External> wrap = Local<External>::Cast(args[0]->ToObject()->GetInternalField(0));
  // cl_device_id device_id = static_cast<cl_device_id>(wrap->Value());
  // cl_int num=::clRetainDevice(device_id);
  // NanReturnValue(JS_INT(num));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseDevice(cl_device_id /* device */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(ReleaseDevice) {
  NanScope();
  REQ_ARGS(1);

  // TODO make sure device_id is a sub-device
  NanReturnValue(JS_INT(0));

  // Local<External> wrap = Local<External>::Cast(args[0]->ToObject()->GetInternalField(0));
  // cl_device_id device_id = static_cast<cl_device_id>(wrap->Value());
  // cl_int num=::clReleaseDevice(device_id);
  // NanReturnValue(JS_INT(num));
}

namespace Device {
void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "getDeviceIDs", GetDeviceIDs);
  NODE_SET_METHOD(exports, "getDeviceInfo", GetDeviceInfo);
  NODE_SET_METHOD(exports, "createSubDevices", CreateSubDevices);
  NODE_SET_METHOD(exports, "retainDevice", RetainDevice);
  NODE_SET_METHOD(exports, "releaseDevice", ReleaseDevice);
}
} // namespace Device

} // namespace opencl
