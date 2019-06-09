#include "nan.h"
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
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(platform_id, NoCLPlatformId, info[0]);

  cl_device_type type = CL_DEVICE_TYPE_ALL;
  if(!info[1]->IsUndefined() && !info[1]->IsNull())
    type=Nan::To<uint32_t>(info[1]).FromJust();

  cl_uint n = 0;
  CHECK_ERR(::clGetDeviceIDs(platform_id->getRaw(), type, 0, NULL, &n));

  unique_ptr<cl_device_id[]> devices(new cl_device_id[n]);
  CHECK_ERR(::clGetDeviceIDs(platform_id->getRaw(), type, n, devices.get(), NULL));

  Local<Array> deviceArray = Nan::New<Array>(n);
  for (uint32_t i=0; i<n; i++) {
    // This is a noop for root-level devices but properly retains sub-devices.
    CHECK_ERR(::clRetainDevice(devices[i]));
    Nan::Set(deviceArray, i, NOCL_WRAP(NoCLDeviceId, devices[i]));
  }

  info.GetReturnValue().Set(deviceArray);
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetDeviceInfo(cl_device_id    /* device */,
//                 cl_device_info  /* param_name */,
//                 size_t          /* param_value_size */,
//                 void *          /* param_value */,
//                 size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetDeviceInfo) {
  // TODO: CHECK ALL VALUES ARE LISTED (I think at least CL_DEVICE_PARENT is missing)
  Nan::HandleScope scope;
  REQ_ARGS(2);

  NOCL_UNWRAP(deviceId, NoCLDeviceId, info[0]);

  cl_device_id device_id = deviceId->getRaw();
  cl_device_info param_name = Nan::To<uint32_t>(info[1]).FromJust();

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
    info.GetReturnValue().Set(JS_STR(param_value,(int)param_value_size_ret - 1));
    return;
  }
  break;
  case CL_DEVICE_PLATFORM: {
    cl_platform_id param_value;

    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_platform_id), &param_value, NULL));

    if(param_value) {
      info.GetReturnValue().Set(NOCL_WRAP(NoCLPlatformId, param_value));
    }
    return;
  }
  break;
  case CL_DEVICE_TYPE: {
    cl_device_type param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_type), &param_value, NULL));
    info.GetReturnValue().Set(JS_INT(param_value));
  }
  return;
  case CL_DEVICE_LOCAL_MEM_TYPE: {
    cl_device_local_mem_type param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_local_mem_type), &param_value, NULL));
    info.GetReturnValue().Set(JS_INT(param_value));
    return;
  }
  case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: {
    cl_device_mem_cache_type param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_mem_cache_type), &param_value, NULL));
    info.GetReturnValue().Set(JS_INT(param_value));
    return;
  }
  case CL_DEVICE_EXECUTION_CAPABILITIES: {
    cl_device_exec_capabilities param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_exec_capabilities), &param_value, NULL));
    info.GetReturnValue().Set(JS_INT((int)param_value));
    return;
  }
  case CL_DEVICE_QUEUE_PROPERTIES: {
    cl_command_queue_properties param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_command_queue_properties), &param_value, NULL));
    info.GetReturnValue().Set(JS_INT((int)param_value));
    return;
  }
  case CL_DEVICE_HALF_FP_CONFIG:
  case CL_DEVICE_SINGLE_FP_CONFIG:
  case CL_DEVICE_DOUBLE_FP_CONFIG: {
    cl_device_fp_config param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_device_fp_config), &param_value, NULL));

    info.GetReturnValue().Set(JS_INT((int)param_value));
    return;
  }
  case CL_DEVICE_MAX_WORK_ITEM_SIZES: {
    // get CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS param
    cl_uint max_work_item_dimensions;
    CHECK_ERR(::clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), &max_work_item_dimensions, NULL));

    // get CL_DEVICE_MAX_WORK_ITEM_SIZES array param
    unique_ptr<size_t[]> param_value(new size_t[max_work_item_dimensions]);
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, max_work_item_dimensions*sizeof(size_t), param_value.get(), NULL));

    Local<Array> arr = Nan::New<Array>(max_work_item_dimensions);
    for(cl_uint i=0;i<max_work_item_dimensions;i++)
      Nan::Set(arr, i,JS_INT(uint32_t(param_value[i])));

    info.GetReturnValue().Set(arr);
    return;
  }
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
    info.GetReturnValue().Set((param_value==CL_TRUE) ? Nan::True() : Nan::False());
    return;
  }
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
  case CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: // TODO: Check this field, specs say this will be deprecated starting from v1.2
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
#if !defined (__APPLE__) && !defined(MACOSX)
  case CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV:
  case CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV:
  case CL_DEVICE_REGISTERS_PER_BLOCK_NV:
  case CL_DEVICE_WARP_SIZE_NV:
  case CL_DEVICE_GPU_OVERLAP_NV:
  case CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV:
  case CL_DEVICE_INTEGRATED_MEMORY_NV:
#endif
  // OpenCL 1.2 constants
#ifdef CL_VERSION_1_2
  case CL_DEVICE_REFERENCE_COUNT:
  case CL_DEVICE_PARTITION_MAX_SUB_DEVICES:
#endif
  // OpenCL 1.2 constants
#ifdef CL_VERSION_2_0
  case CL_DEVICE_MAX_PIPE_ARGS:
  case CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS:
  case CL_DEVICE_PIPE_MAX_PACKET_SIZE:
#endif
  {
    cl_uint param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_uint), &param_value, NULL));
    info.GetReturnValue().Set(JS_INT((int)param_value));
    return;
  }
  // cl_ulong params
  case CL_DEVICE_LOCAL_MEM_SIZE:
  case CL_DEVICE_GLOBAL_MEM_SIZE:
  case CL_DEVICE_MAX_MEM_ALLOC_SIZE:
  case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:
  case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:
  {
    cl_ulong param_value;
    CHECK_ERR(::clGetDeviceInfo(device_id, param_name, sizeof(cl_ulong), &param_value, NULL));

    /**
      JS Compatibility

      As JS does not support 64 bits integer, we return the amount as a 32 bit value containing the amount in kilobytes, it should be enough for couple of generations of GPUs, in future the value can be returned in megabytes / gigabytes / etc. with library's major version bump.
    */
    info.GetReturnValue().Set(JS_INT((uint32_t) (param_value >> 10)));

    return;
  }
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

    // assume for these params it will fit in an int
    info.GetReturnValue().Set(Nan::New<Integer>((int)param_value));
    return;
  }
  default: {
    THROW_ERR(CL_INVALID_VALUE);
  }
  }
  return;
}

#ifdef CL_VERSION_1_2
// extern CL_API_ENTRY cl_int CL_API_CALL
// clCreateSubDevices(cl_device_id                         /* in_device */,
//                    const cl_device_partition_property * /* properties */,
//                    cl_uint                              /* num_devices */,
//                    cl_device_id *                       /* out_devices */,
//                    cl_uint *                            /* num_devices_ret */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(CreateSubDevices) {

  Nan::HandleScope scope;
  REQ_ARGS(2)

  // Arg 1
  NOCL_UNWRAP(device, NoCLDeviceId, info[0]);
  cl_device_id deviceId = device->getRaw();

  // Arg 2
  std::vector<cl_device_partition_property> cl_properties;
  REQ_ARRAY_ARG(1, js_properties);
  for (unsigned int i = 0; i < js_properties->Length(); ++ i) {
    cl_properties.push_back(Nan::To<int32_t>(Nan::Get(js_properties, i).ToLocalChecked()).FromJust());
  }

  cl_uint capacity = 0;
  cl_device_partition_property pps [] = {CL_DEVICE_PARTITION_BY_COUNTS,3,1,CL_DEVICE_PARTITION_BY_COUNTS_LIST_END,0};

  cl_int ret = ::clCreateSubDevices(deviceId, pps, 0, NULL, &capacity);
  CHECK_ERR(ret);
  unique_ptr<cl_device_id[]> subDevices(new cl_device_id[capacity]);
  ret = ::clCreateSubDevices(deviceId, &cl_properties.front(), capacity, subDevices.get(), NULL);
  CHECK_ERR(ret);

  Local<Array> subDevicesArray = Nan::New<Array>(capacity);
  for (uint32_t i=0; i<capacity; i++) {
    Nan::Set(subDevicesArray, i, NOCL_WRAP(NoCLDeviceId, subDevices[i]));
  }

  info.GetReturnValue().Set(subDevicesArray);

}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainDevice(cl_device_id /* device */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(RetainDevice) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(device, NoCLDeviceId, info[0]);

  cl_device_id deviceId = device->getRaw();
  cl_device_id parentId = NULL;

  ::clGetDeviceInfo(deviceId, CL_DEVICE_PARENT_DEVICE, sizeof(cl_device_id), &parentId, NULL);

  if (parentId == NULL) {
    THROW_ERR(CL_INVALID_DEVICE);
  }

  cl_int ret = ::clRetainDevice(deviceId);

  CHECK_ERR(ret);
  info.GetReturnValue().Set(JS_INT(ret));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseDevice(cl_device_id /* device */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(ReleaseDevice) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(device, NoCLDeviceId, info[0]);

  cl_device_id deviceId = device->getRaw();
  cl_device_id parentId = NULL;

  ::clGetDeviceInfo(deviceId, CL_DEVICE_PARENT_DEVICE, sizeof(cl_device_id), &parentId, NULL);

  if (parentId == NULL) {
    THROW_ERR(CL_INVALID_DEVICE);
  }

  cl_int ret = ::clReleaseDevice(deviceId);
  CHECK_ERR(ret);
  info.GetReturnValue().Set(JS_INT(ret));
}
#endif

#ifdef CL_VERSION_2_1
// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetDeviceAndHostTimer(cl_device_id    /* device */,
//                         cl_ulong*       /* device_timestamp */,
//                         cl_ulong*       /* host_timestamp */) CL_API_SUFFIX__VERSION_2_1;

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetHostTimer(cl_device_id /* device */,
//                cl_ulong *   /* host_timestamp */)  CL_API_SUFFIX__VERSION_2_1;
#endif

namespace Device {
NAN_MODULE_INIT(init)
{
  Nan::SetMethod(target, "getDeviceIDs", GetDeviceIDs);
  Nan::SetMethod(target, "getDeviceInfo", GetDeviceInfo);
#ifdef CL_VERSION_1_2
  Nan::SetMethod(target, "createSubDevices", CreateSubDevices);
  Nan::SetMethod(target, "retainDevice", RetainDevice);
  Nan::SetMethod(target, "releaseDevice", ReleaseDevice);
#endif
#ifdef CL_VERSION_2_1
  // @TODO Nan::SetMethod(target, "getDeviceAndHostTimer", GetDeviceAndHostTimer);
  // @TODO Nan::SetMethod(target, "getHostTimer", GetHostTimer);
#endif
}
} // namespace Device

} // namespace opencl
