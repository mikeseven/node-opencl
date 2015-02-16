#include "program.h"
#include "types.h"
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

  NOCL_UNWRAP(context, NoCLContext, args[0]);
  REQ_STR_ARG(1, str);

  cl_int ret=CL_SUCCESS;
  size_t lengths[]={(size_t) str.length()};
  const char *strings[]={*str};
  cl_program p=::clCreateProgramWithSource(context->getRaw(), 1, strings, lengths, &ret);
  CHECK_ERR(ret);

  NanReturnValue(NOCL_WRAP(NoCLProgram, p));
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

  NOCL_UNWRAP(context, NoCLContext, args[0]);

  vector<NoCLDeviceId> cl_devices;
  REQ_ARRAY_ARG(1, devices);
  NOCL_TO_ARRAY(cl_devices, devices, NoCLDeviceId);

  for (uint32_t i=0; i < devices->Length(); i++) {
    NOCL_UNWRAP(device, NoCLDeviceId, devices->Get(i));
    cl_devices.push_back(device->getRaw());
  }

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
              context->getRaw(), (cl_uint) cl_devices.size(),
              TO_CL_ARRAY(cl_devices, NoCLDeviceId),
              lengths.get(), images.get(),
              NULL, &ret);
  CHECK_ERR(ret);

  NanReturnValue(NOCL_WRAP(NoCLProgram, p));
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

  NOCL_UNWRAP(p, NoCLProgram, args[0]);
  cl_int count=clRetainProgram(p->getRaw());

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseProgram(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseProgram) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(p, NoCLProgram, args[0]);
  cl_int count=clReleaseProgram(p->getRaw());

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

  NOCL_UNWRAP(p, NoCLProgram, args[0]);

  std::vector<NoCLDeviceId> devices;

  if (ARG_EXISTS(1)){
    REQ_ARRAY_ARG(1, cl_devices);
    NOCL_TO_ARRAY(devices, cl_devices, NoCLDeviceId);
  }
  
  String::Utf8Value * options = nullptr;

  if (ARG_EXISTS(2)){
    if (!args[2]->IsString()) {
      throwTypeMismatch(2, "options", "string");
    }
    options = new String::Utf8Value(args[2]);
  }

  //REQ_STR_ARG(2,options);

  // TODO callback + userdata

  int err = ::clBuildProgram(p->getRaw(),
    devices.size(), TO_CL_ARRAY(devices, NoCLDeviceId),
    options != NULL ? **options : nullptr,
    nullptr, nullptr);

  delete options;

  CHECK_ERR(err); // TODO CB

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
  REQ_ARGS(1);

  // Arg 1 : program
  NOCL_UNWRAP(p, NoCLProgram, args[0]);

  // Arg 2 : devices
  std::vector<NoCLDeviceId> cl_devices;
  REQ_ARRAY_ARG(1, js_devices);
  NOCL_TO_ARRAY(cl_devices, js_devices, NoCLDeviceId);

  // Arg 3 : Options
  String::Utf8Value * options = nullptr;

  if (ARG_EXISTS(2)){
    if (!args[2]->IsString()) {
      throwTypeMismatch(2, "options", "string");
    }
    options = new String::Utf8Value(args[2]);
  }

  // Arg 4 : programs included
  // Arg 5 : headers names
  std::vector<NoCLProgram> program_headers;
  std::vector<const char *> names;


  // Checking correct mapping
  if (ARG_EXISTS(3)){
    REQ_ARRAY_ARG(3, js_programs);
    NOCL_TO_ARRAY(program_headers, js_programs, NoCLProgram);
  }

  if (ARG_EXISTS(4)){
    Local<Array> arr = Local<Array>::Cast(args[4]);
    for (unsigned int i = 0; i < arr->Length(); ++ i) {
      String::Utf8Value str(arr->Get(i));
      names.push_back(str.operator*());
    }
  }

  // Arg 4 and 5 mapping
  if (program_headers.size() != names.size()) {
    NanThrowError("Program headers and names should be of the same size");
  }

  // CL CALL
  // TODO Callbacks

  int err = ::clCompileProgram(
    p->getRaw(),
    cl_devices.size(), TO_CL_ARRAY(cl_devices, NoCLDeviceId),
    options != NULL ? **options : nullptr,
    program_headers.size(), TO_CL_ARRAY(program_headers, NoCLProgram),
    &names.front(),
    nullptr, nullptr);

  delete options;

  CHECK_ERR(err);

  NanReturnValue(JS_INT(CL_SUCCESS));
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
  REQ_ARGS(1);


  // Arg 1
  NOCL_UNWRAP(platform, NoCLPlatformId, args[0]);

  CHECK_ERR(::clUnloadPlatformCompiler(platform->getRaw()));
  NanReturnValue(JS_INT(CL_SUCCESS));
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

  NOCL_UNWRAP(program, NoCLProgram, args[0]);
  if(!isOpenCLObj(args[0])) {
    THROW_ERR(CL_INVALID_MEM_OBJECT);
  }
  cl_program prog=Unwrap<cl_program>(args[0]);
  cl_program_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_PROGRAM_REFERENCE_COUNT:
    case CL_PROGRAM_NUM_DEVICES:
    {
      cl_uint val;
      CHECK_ERR(::clGetProgramInfo(program->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_PROGRAM_CONTEXT:
    {
      cl_context val;
      CHECK_ERR(::clGetProgramInfo(prog,param_name,sizeof(cl_context), &val, NULL))
      NanReturnValue(Wrap<cl_context>(val));
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
      CHECK_ERR(::clGetProgramInfo(program->getRaw(), param_name, 0, NULL, &nsizes));

      // get CL_DEVICE_MAX_WORK_ITEM_SIZES array param
      unique_ptr<size_t[]> sizes(new size_t[nsizes]);
      CHECK_ERR(::clGetProgramInfo(program->getRaw(), param_name, nsizes*sizeof(size_t), sizes.get(), NULL));
      Local<Array> arr = NanNew<Array>(nsizes);
      for(cl_uint i=0;i<nsizes;i++)
        arr->Set(i,JS_INT(sizes[i]));

      NanReturnValue(arr);
    }
    case CL_PROGRAM_BINARIES:
    {
      size_t nbins=0;
      CHECK_ERR(::clGetProgramInfo(program->getRaw(), param_name, 0, NULL, &nbins));
      nbins /= sizeof(size_t);
      unique_ptr<char*[]> binaries(new char*[nbins]);
      CHECK_ERR(::clGetProgramInfo(program->getRaw(), param_name, sizeof(char*)*nbins, binaries.get(), NULL));

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
      CHECK_ERR(::clGetProgramInfo(program->getRaw(),param_name,sizeof(size_t), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_PROGRAM_SOURCE:
    case CL_PROGRAM_KERNEL_NAMES:
    {
      size_t nchars;
      CHECK_ERR(::clGetProgramInfo(program->getRaw(), param_name, 0, NULL, &nchars));
      unique_ptr<char[]> names(new char[nchars]);
      CHECK_ERR(::clGetProgramInfo(program->getRaw(), param_name, nchars*sizeof(char), names.get(), NULL));
      NanReturnValue(JS_STR(names.get()));
    }
  }
  THROW_ERR(CL_INVALID_VALUE);
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

  NOCL_UNWRAP(prog, NoCLProgram, args[0]);
  NOCL_UNWRAP(device, NoCLDeviceId, args[1]);
  cl_program_build_info param_name = args[2]->Uint32Value();

  switch(param_name) {
    case CL_PROGRAM_BUILD_STATUS:
    {
      cl_build_status val;
      CHECK_ERR(::clGetProgramBuildInfo(prog->getRaw(), device->getRaw(),param_name,sizeof(cl_build_status), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_PROGRAM_BUILD_OPTIONS:
    case CL_PROGRAM_BUILD_LOG:
    {
      size_t param_value_size_ret=0;
      CHECK_ERR(::clGetProgramBuildInfo(prog->getRaw(), device->getRaw(), param_name, 0, NULL, &param_value_size_ret));
      unique_ptr<char[]> param_value(new char[param_value_size_ret]);
      CHECK_ERR(::clGetProgramBuildInfo(prog->getRaw(), device->getRaw(), param_name, param_value_size_ret, param_value.get(), NULL));
      NanReturnValue(JS_STR(param_value.get(),(int)param_value_size_ret));
    }
    case CL_PROGRAM_BINARY_TYPE:
    {
      cl_program_binary_type val;
      CHECK_ERR(::clGetProgramBuildInfo(prog->getRaw(), device->getRaw(), param_name,sizeof(cl_program_binary_type), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
  }
  THROW_ERR(CL_INVALID_VALUE);
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
