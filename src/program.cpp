#include "program.h"
#include <vector>

namespace opencl {

// /* Program Object APIs  */
// extern CL_API_ENTRY cl_program CL_API_CALL
// clCreateProgramWithSource(cl_context        /* context */,
//                           cl_uint           /* count */,
//                           const char **     /* strings */,
//                           const size_t *    /* lengths */,
//                           cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateProgramWithSource) {
  NanScope();
  REQ_ARGS(2);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_CONTEXT));
  }
  cl_context context=Unwrap<cl_context>(args[0]);
  REQ_STR_ARG(1, str);

  cl_int ret=CL_SUCCESS;
  size_t lengths[]={(size_t) str.length()};
  const char *strings[]={*str};
  cl_program p=::clCreateProgramWithSource(context, 1, strings, lengths, &ret);
  CHECK_ERR(ret);

  NanReturnValue(Wrap(p));
}

// extern CL_API_ENTRY cl_program CL_API_CALL
// clCreateProgramWithBinary(cl_context                     /* context */,
//                           cl_uint                        /* num_devices */,
//                           const cl_device_id *           /* device_list */,
//                           const size_t *                 /* lengths */,
//                           const unsigned char **         /* binaries */,
//                           cl_int *                       /* binary_status */,
//                           cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateProgramWithBinary) {
  NanScope();
  REQ_ARGS(3);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_CONTEXT));
  }
  cl_context context=Unwrap<cl_context>(args[0]);

  Local<Array> devices = Local<Array>::Cast(args[1]);
  vector<cl_device_id> cl_devices;
  for (uint32_t i=0; i < devices->Length(); i++)
    cl_devices.push_back(Unwrap<cl_device_id>(devices->Get(i)));

  Local<Array> binaries = Local<Array>::Cast(args[2]);
  const size_t n = binaries->Length();
  unique_ptr<size_t[]> lengths(new size_t[n]);
  unique_ptr<const unsigned char*[]> images(new const unsigned char*[n]);

  for (uint32_t i = 0; i < n; ++i) {
    Local<Object> obj=binaries->Get(i)->ToObject();
    images[i] = (const unsigned char*) obj->GetIndexedPropertiesExternalArrayData();
    lengths[i] = obj->GetIndexedPropertiesExternalArrayDataLength();
  }

  cl_int ret=CL_SUCCESS;
  cl_program p=::clCreateProgramWithBinary(
              context, (cl_uint) cl_devices.size(),
              &cl_devices.front(),
              lengths.get(), images.get(),
              NULL, &ret);
  CHECK_ERR(ret);

  NanReturnValue(Wrap(p));
}

// extern CL_API_ENTRY cl_program CL_API_CALL
// clCreateProgramWithBuiltInKernels(cl_context            /* context */,
//                                   cl_uint               /* num_devices */,
//                                   const cl_device_id *  /* device_list */,
//                                   const char *          /* kernel_names */,
//                                   cl_int *              /* errcode_ret */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(CreateProgramWithBuiltInKernels) {
  NanScope();
  // TODO
  NanReturnUndefined();

}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainProgram(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainProgram) {
  NanScope();
  REQ_ARGS(1);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_PROGRAM));
  }

  cl_program p = Unwrap<cl_program>(args[0]);
  cl_int count=clRetainProgram(p);

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseProgram(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseProgram) {
  NanScope();
  REQ_ARGS(1);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_PROGRAM));
  }

  cl_program p = Unwrap<cl_program>(args[0]);
  cl_int count=clRetainProgram(p);

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clBuildProgram(cl_program           /* program */,
//                cl_uint              /* num_devices */,
//                const cl_device_id * /* device_list */,
//                const char *         /* options */,
//                void (CL_CALLBACK *  /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
//                void *               /* user_data */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(BuildProgram) {
  NanScope();
  REQ_ARGS(1);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_PROGRAM));
  }

  cl_program p = Unwrap<cl_program>(args[0]);
  std::vector<cl_device_id> devices;
  getValuesFromArray(Local<Array>::Cast(args[1]),devices);
  REQ_STR_ARG(2,options);

  // TODO callback + userdata

  CHECK_ERR(::clBuildProgram(p,
    devices.size(), devices.size() ? &devices.front() : nullptr,
    *options,
    nullptr, nullptr)); // TODO CB

  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clCompileProgram(cl_program           /* program */,
//                  cl_uint              /* num_devices */,
//                  const cl_device_id * /* device_list */,
//                  const char *         /* options */,
//                  cl_uint              /* num_input_headers */,
//                  const cl_program *   /* input_headers */,
//                  const char **        /* header_include_names */,
//                  void (CL_CALLBACK *  /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
//                  void *               /* user_data */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(CompileProgram) {
  NanScope();
  // TODO
  NanReturnUndefined();

}

// extern CL_API_ENTRY cl_program CL_API_CALL
// clLinkProgram(cl_context           /* context */,
//               cl_uint              /* num_devices */,
//               const cl_device_id * /* device_list */,
//               const char *         /* options */,
//               cl_uint              /* num_input_programs */,
//               const cl_program *   /* input_programs */,
//               void (CL_CALLBACK *  /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
//               void *               /* user_data */,
//               cl_int *             /* errcode_ret */ ) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(LinkProgram) {
  NanScope();
  // TODO
  NanReturnUndefined();

}


// extern CL_API_ENTRY cl_int CL_API_CALL
// clUnloadPlatformCompiler(cl_platform_id /* platform */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(UnloadPlatformCompiler) {
  NanScope();
  // TODO
  NanReturnUndefined();

}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetProgramInfo(cl_program         /* program */,
//                  cl_program_info    /* param_name */,
//                  size_t             /* param_value_size */,
//                  void *             /* param_value */,
//                  size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetProgramInfo) {
  NanScope();
  REQ_ARGS(2);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_program prog=Unwrap<cl_program>(args[0]);
  cl_program_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_PROGRAM_REFERENCE_COUNT:
    case CL_PROGRAM_NUM_DEVICES:
    {
      cl_uint val;
      CHECK_ERR(::clGetProgramInfo(prog,param_name,sizeof(cl_uint), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_PROGRAM_CONTEXT:
    {
      cl_context val;
      CHECK_ERR(::clGetProgramInfo(prog,param_name,sizeof(cl_context), &val, NULL))
      // TODO NanReturnValue(JS_INT(val));
      break;
    }
    case CL_PROGRAM_DEVICES:
    {
      // TODO
      break;
    }
    case CL_PROGRAM_BINARY_SIZES:
    {
      size_t nsizes;
      CHECK_ERR(::clGetProgramInfo(prog, param_name, 0, NULL, &nsizes));

      // get CL_DEVICE_MAX_WORK_ITEM_SIZES array param
      unique_ptr<size_t[]> sizes(new size_t[nsizes]);
      CHECK_ERR(::clGetProgramInfo(prog, param_name, nsizes*sizeof(size_t), sizes.get(), NULL));
      Local<Array> arr = Array::New(nsizes);
      for(cl_uint i=0;i<nsizes;i++)
        arr->Set(i,JS_INT(uint32_t(sizes[i])));

      NanReturnValue(arr);
    }
    case CL_PROGRAM_BINARIES:
    {
      size_t nbins=0;
      CHECK_ERR(::clGetProgramInfo(prog, param_name, 0, NULL, &nbins));
      nbins /= sizeof(size_t);
      unique_ptr<char*[]> binaries(new char*[nbins]);
      CHECK_ERR(::clGetProgramInfo(prog, param_name, sizeof(char*)*nbins, binaries.get(), NULL));

      // TODO create an array for Uint8Array to return each binary associated to each device
      // Handle<Value> abv = Context::GetCurrent()->Global()->Get(String::NewSymbol("ArrayBuffer"));
      // Handle<Value> argv[] = { Integer::NewFromUnsigned(size) };
      // Handle<Object> arrbuf = Handle<Function>::Cast(abv)->NewInstance(1, argv);
      // void *buffer = arrbuf->GetPointerFromInternalField(0);
      // memcpy(buffer, data, size);

      NanReturnUndefined();
    }
    case CL_PROGRAM_NUM_KERNELS:
    {
      size_t val;
      CHECK_ERR(::clGetProgramInfo(prog,param_name,sizeof(size_t), &val, NULL))
      NanReturnValue(JS_INT(uint32_t(val)));
    }
    case CL_PROGRAM_SOURCE:
    case CL_PROGRAM_KERNEL_NAMES:
    {
      size_t nchars;
      CHECK_ERR(::clGetProgramInfo(prog, param_name, 0, NULL, &nchars));
      unique_ptr<char[]> names(new char[nchars]);
      CHECK_ERR(::clGetProgramInfo(prog, param_name, nchars*sizeof(char), names.get(), NULL));
      NanReturnValue(JS_STR(names.get()));
    }
  }
  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetProgramBuildInfo(cl_program            /* program */,
//                       cl_device_id          /* device */,
//                       cl_program_build_info /* param_name */,
//                       size_t                /* param_value_size */,
//                       void *                /* param_value */,
//                       size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetProgramBuildInfo) {
  NanScope();
  REQ_ARGS(3);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_MEM_OBJECT));
  }
  cl_program prog=Unwrap<cl_program>(args[0]);
  if(!isOpenCLObj(args[1])) {
    return NanThrowError(JS_INT(CL_INVALID_DEVICE));
  }
  cl_device_id device=Unwrap<cl_device_id>(args[1]);
  cl_program_build_info param_name = args[2]->Uint32Value();

  switch(param_name) {
    case CL_PROGRAM_BUILD_STATUS:
    {
      cl_build_status val;
      CHECK_ERR(::clGetProgramBuildInfo(prog, device,param_name,sizeof(cl_build_status), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_PROGRAM_BUILD_OPTIONS:
    case CL_PROGRAM_BUILD_LOG:
    {
      size_t param_value_size_ret=0;
      CHECK_ERR(::clGetProgramBuildInfo(prog, device, param_name, 0, NULL, &param_value_size_ret));
      unique_ptr<char[]> param_value(new char[param_value_size_ret]);
      CHECK_ERR(::clGetProgramBuildInfo(prog, device, param_name, param_value_size_ret, param_value.get(), NULL));
      NanReturnValue(JS_STR(param_value.get(),(int)param_value_size_ret));
    }
    case CL_PROGRAM_BINARY_TYPE:
    {
      cl_program_binary_type val;
      CHECK_ERR(::clGetProgramBuildInfo(prog, device,param_name,sizeof(cl_program_binary_type), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
  }
  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

namespace Program {
void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "createProgramWithSource", CreateProgramWithSource);
  NODE_SET_METHOD(exports, "createProgramWithBinary", CreateProgramWithBinary);
  NODE_SET_METHOD(exports, "createProgramWithBuiltInKernels", CreateProgramWithBuiltInKernels);
  NODE_SET_METHOD(exports, "retainProgram", RetainProgram);
  NODE_SET_METHOD(exports, "releaseProgram", ReleaseProgram);
  NODE_SET_METHOD(exports, "buildProgram", BuildProgram);
  NODE_SET_METHOD(exports, "compileProgram", CompileProgram);
  NODE_SET_METHOD(exports, "linkProgram", LinkProgram);
  NODE_SET_METHOD(exports, "unloadPlatformCompiler", UnloadPlatformCompiler);
  NODE_SET_METHOD(exports, "getProgramInfo", GetProgramInfo);
  NODE_SET_METHOD(exports, "getProgramBuildInfo", GetProgramBuildInfo);
}
} // namespace Program

} // namespace opencl
