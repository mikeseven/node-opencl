#include "program.h"
#include "types.h"
#include <vector>
#include "nanextension.h"

namespace opencl {

// /* Program Object APIs  */

// extern CL_API_ENTRY cl_program CL_API_CALL
// clCreateProgramWithSource(cl_context        /* context */,
//                           cl_uint           /* count */,
//                           const char **     /* strings */,
//                           const size_t *    /* lengths */,
//                           cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateProgramWithSource) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  NOCL_UNWRAP(context, NoCLContext, info[0]);
  REQ_STR_ARG(1, str);

  cl_int ret=CL_SUCCESS;
  size_t lengths[]={(size_t) str.length()};
  const char *strings[]={*str};
  cl_program p=::clCreateProgramWithSource(context->getRaw(), 1, strings, lengths, &ret);
  CHECK_ERR(ret);

  info.GetReturnValue().Set(NOCL_WRAP(NoCLProgram, p));
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
  Nan::HandleScope scope;
  REQ_ARGS(3);

  NOCL_UNWRAP(context, NoCLContext, info[0]);

  vector<NoCLDeviceId *> cl_devices;
  REQ_ARRAY_ARG(1, devices);
  NOCL_TO_ARRAY(cl_devices, devices, NoCLDeviceId);

  Local<Array> js_sizes = Local<Array>::Cast(info[2]);
  if (js_sizes->Length() == 0) {
    THROW_ERR(CL_INVALID_VALUE)
  }
  
  const size_t n = js_sizes->Length();

  std::vector<size_t> cl_binary_lengths;
  unique_ptr<size_t[]> originalLengths(new size_t[n]);

  for (unsigned int i = 0; i < n; ++ i) {
    int32_t len = Nan::To<int32_t>(Nan::Get(js_sizes, i).ToLocalChecked()).FromJust();
    originalLengths[i] = len;
    if (len > 0) {
      cl_binary_lengths.push_back(len);
    }
  }

  if (cl_binary_lengths.size() == 0) {
    THROW_ERR(CL_INVALID_PROGRAM_EXECUTABLE)
  }

  std::vector<NoCLProgramBinary *> cl_binaries;
  std::vector<unsigned const char *> cl_binaries_str;

  REQ_ARRAY_ARG(3, js_binaries);
  NOCL_TO_ARRAY(cl_binaries, js_binaries, NoCLProgramBinary);

  if (js_sizes->Length() != js_binaries->Length()) {
    THROW_ERR(CL_INVALID_VALUE)
  }

  for (unsigned int i = 0; i < n; ++ i) {
    int32_t len = originalLengths[i];
    if (len > 0) {
      cl_binaries_str.push_back(cl_binaries[i]->getRaw());
    }
  }

  cl_int ret=CL_SUCCESS;
  cl_program p=::clCreateProgramWithBinary(
    context->getRaw(),
    (cl_uint) cl_devices.size(),
    NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
    &cl_binary_lengths[0],
    &cl_binaries_str[0],
    NULL,
    &ret);

  CHECK_ERR(ret);

  info.GetReturnValue().Set(NOCL_WRAP(NoCLProgram, p));
}

#ifdef CL_VERSION_1_2
// extern CL_API_ENTRY cl_program CL_API_CALL
// clCreateProgramWithBuiltInKernels(cl_context            /* context */,
//                                   cl_uint               /* num_devices */,
//                                   const cl_device_id *  /* device_list */,
//                                   const char *          /* kernel_names */,
//                                   cl_int *              /* errcode_ret */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(CreateProgramWithBuiltInKernels) {
  Nan::HandleScope scope;
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 1
  vector<NoCLDeviceId *> cl_devices;
  REQ_ARRAY_ARG(1, devices);
  NOCL_TO_ARRAY(cl_devices, devices, NoCLDeviceId);

  // Arg 2
  vector<char *> names;
  REQ_ARRAY_ARG(2, js_names);
  for (unsigned int i=0; i < js_names->Length(); ++ i) {
    if (!js_names->IsString()) {
      THROW_ERR(CL_INVALID_VALUE);
    }
    names.push_back(*Nan::Utf8String(Nan::Get(js_names, i).ToLocalChecked()));
  }

  if (names.size() == 0) {
    THROW_ERR(CL_INVALID_VALUE);
  }

  cl_int err = CL_SUCCESS;
  cl_program prg = ::clCreateProgramWithBuiltInKernels(
    context->getRaw(),
    (cl_uint) cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
    names.front(),
    &err);

  CHECK_ERR(err);

  info.GetReturnValue().Set(NOCL_WRAP(NoCLProgram, prg));
}
#endif
// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainProgram(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainProgram) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(p, NoCLProgram, info[0]);
  cl_int err=p->acquire();
  CHECK_ERR(err);
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseProgram(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseProgram) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(p, NoCLProgram, info[0]);
  cl_int err=p->release();
  CHECK_ERR(err);
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

class NoCLProgramWorker : public AsyncWorker
{
public:
  NoCLProgramWorker(Callback* callback, const v8::Local<v8::Object> &userData,const v8::Local<v8::Object> &noCLProgram) :
  AsyncWorker(callback)
  {
    SaveToPersistent(kIndex,  userData);
    SaveToPersistent(kIndex+1,noCLProgram);
  }

  ~NoCLProgramWorker() {}

  // Executed inside the worker-thread.
  // not safe to use V8 calls
  void Execute()
  {
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    Nan::EscapableHandleScope  scope;

    Local<Value> argv[] = {
      GetFromPersistent(kIndex),  // event
      GetFromPersistent(kIndex+1) // userdata
    };
    Nan::Call(*callback, 2, argv);
  }

protected:
  static const uint32_t kIndex = 0;
};

void CL_CALLBACK notifyPCB (cl_program prog, void *user_data) {
  NoCLProgramWorker* asyncCB = static_cast<NoCLProgramWorker*>(user_data);
  AsyncQueueWorker(asyncCB);
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clBuildProgram(cl_program           /* program */,
//                cl_uint              /* num_devices */,
//                const cl_device_id * /* device_list */,
//                const char *         /* options */,
//                void (CL_CALLBACK *  /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
//                void *               /* user_data */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(BuildProgram) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(p, NoCLProgram, info[0]);

  std::vector<NoCLDeviceId *> devices;

  if (ARG_EXISTS(1)){
    REQ_ARRAY_ARG(1, cl_devices);
    NOCL_TO_ARRAY(devices, cl_devices, NoCLDeviceId);
  }

  Nan::Utf8String * options = nullptr;

  if (ARG_EXISTS(2)){
    if (!info[2]->IsString()) {
      THROW_ERR(CL_INVALID_BUILD_OPTIONS)
    }
    options = new Nan::Utf8String(info[2]);
  }

  //  callback + userdata
  int err;

  if (ARG_EXISTS(3)) {
    Local<Function> callbackHandle = info[3].As<Function>();
    Nan::Callback *callback = new Nan::Callback(callbackHandle);
    NoCLProgramWorker* cb = new NoCLProgramWorker(callback,info[4].As<Object>(),info[0].As<Object>());

   err = ::clBuildProgram(p->getRaw(),
          (cl_uint) devices.size(), NOCL_TO_CL_ARRAY(devices, NoCLDeviceId),
          options != NULL ? **options : nullptr,
          notifyPCB, cb);
  }
  else
    err = ::clBuildProgram(p->getRaw(),
          (cl_uint) devices.size(), NOCL_TO_CL_ARRAY(devices, NoCLDeviceId),
          options != NULL ? **options : nullptr,
          nullptr, nullptr);

  if(options) delete options;


  CHECK_ERR(err);


  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

#ifdef CL_VERSION_1_2
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
  Nan::HandleScope scope;
  REQ_ARGS(1);

  // Arg 1 : program
  NOCL_UNWRAP(p, NoCLProgram, info[0]);

  std::vector<NoCLDeviceId *> cl_devices;

  // Arg 2 : devices
  if (ARG_EXISTS(1)) {
    REQ_ARRAY_ARG(1, js_devices);
    NOCL_TO_ARRAY(cl_devices, js_devices, NoCLDeviceId);
  }

  // Arg 3 : Options
  Nan::Utf8String * options = nullptr;

  if (ARG_EXISTS(2)){
    if (!info[2]->IsString()) {
      THROW_ERR(CL_INVALID_COMPILER_OPTIONS)
    }
    options = new Nan::Utf8String(info[2]);
  }

  // Arg 4 : programs included
  // Arg 5 : headers names
  std::vector<NoCLProgram *> program_headers;
  std::vector<const char *> names;

  // Checking correct mapping
  if (ARG_EXISTS(3)){
    REQ_ARRAY_ARG(3, js_programs);
    NOCL_TO_ARRAY(program_headers, js_programs, NoCLProgram);
  }

  if (ARG_EXISTS(4)){
    Local<Array> arr = Local<Array>::Cast(info[4]);
    for (unsigned int i = 0; i < arr->Length(); ++ i) {
      Nan::Utf8String str(Nan::Get(arr, i).ToLocalChecked());
      names.push_back(str.operator*());
    }
  }

  // Arg 4 and 5 mapping
  if (program_headers.size() != names.size()) {
    THROW_ERR(CL_INVALID_VALUE);
  }

  // CL CALL
  //  callback + userdata
  int err;

  if (ARG_EXISTS(5)) {
    Local<Function> callbackHandle = info[5].As<Function>();

    Nan::Callback *callback = new Nan::Callback(callbackHandle);
    NoCLProgramWorker* cb = new NoCLProgramWorker(callback,info[6].As<Object>(),info[0].As<Object>());

    err = ::clCompileProgram(
              p->getRaw(),
              (cl_uint) cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
              options != NULL ? **options : nullptr,
              (cl_uint) program_headers.size(), NOCL_TO_CL_ARRAY(program_headers, NoCLProgram),
              &names.front(),
              notifyPCB, cb);
  }


  else
    err = ::clCompileProgram(
              p->getRaw(),
              (cl_uint) cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
              options != NULL ? **options : nullptr,
              (cl_uint) program_headers.size(), NOCL_TO_CL_ARRAY(program_headers, NoCLProgram),
              &names.front(),
              nullptr, nullptr);

  if(options) delete options;

  CHECK_ERR(err);

  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
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
  Nan::HandleScope scope;
  REQ_ARGS(4);


  //Arg 0
  NOCL_UNWRAP(ctx, NoCLContext, info[0]);

  std::vector<NoCLDeviceId *> cl_devices;

  //Arg 1
  if (ARG_EXISTS(1)) {
    REQ_ARRAY_ARG(1, js_devices);
    NOCL_TO_ARRAY(cl_devices, js_devices, NoCLDeviceId);
  }

  //Arg 2
  Nan::Utf8String * options = nullptr;

  if (ARG_EXISTS(2)){
    if (!info[2]->IsString()) {
      THROW_ERR(CL_INVALID_COMPILER_OPTIONS)
    }
    options = new Nan::Utf8String(info[2]);
  }

  //Arg 3
  std::vector<NoCLProgram *> cl_programs;

  if (ARG_EXISTS(3)) {
    REQ_ARRAY_ARG(3, js_programs);
    NOCL_TO_ARRAY(cl_programs, js_programs, NoCLProgram);
  }

  /** OSX ISSUE
  * ret is always equals to CL_SUCCESS, whatever happens when clLinkProgram is called
  * I don't know if anything happens during this call
  */
  cl_int ret = CL_SUCCESS;

  cl_program prg;

  if (ARG_EXISTS(4)) {
    Local<Function> callbackHandle = info[4].As<Function>();
    Nan::Callback *callback = new Nan::Callback(callbackHandle);
    NoCLProgramWorker* cb = new NoCLProgramWorker(callback,info[5].As<Object>(),info[0].As<Object>());

    prg = ::clLinkProgram(
              ctx->getRaw(),
              (cl_uint) cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
              options != NULL ? **options : nullptr,
              (cl_uint) cl_programs.size(), NOCL_TO_CL_ARRAY(cl_programs, NoCLProgram),
              notifyPCB, cb,
              &ret);
  }

  else
    prg = ::clLinkProgram(
              ctx->getRaw(),
              (cl_uint) cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
              options != NULL ? **options : nullptr,
              (cl_uint) cl_programs.size(), NOCL_TO_CL_ARRAY(cl_programs, NoCLProgram),
              NULL, NULL,
              &ret);

  CHECK_ERR(ret);
  info.GetReturnValue().Set(NOCL_WRAP(NoCLProgram, prg));
}
#endif

// extern CL_API_ENTRY cl_int CL_API_CALL
// clUnloadPlatformCompiler(cl_platform_id /* platform */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(UnloadPlatformCompiler) {
  Nan::HandleScope scope;
#ifdef CL_VERSION_1_2
  REQ_ARGS(1);


  // Arg 1
  NOCL_UNWRAP(platform, NoCLPlatformId, info[0]);

  CHECK_ERR(::clUnloadPlatformCompiler(platform->getRaw()));
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
#else
  CHECK_ERR(::clUnloadCompiler());
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
#endif

}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetProgramInfo(cl_program         /* program */,
//                  cl_program_info    /* param_name */,
//                  size_t             /* param_value_size */,
//                  void *             /* param_value */,
//                  size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetProgramInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  NOCL_UNWRAP(prog, NoCLProgram, info[0]);

  cl_program_info param_name = Nan::To<uint32_t>(info[1]).FromJust();

  switch(param_name) {
    case CL_PROGRAM_REFERENCE_COUNT:
    case CL_PROGRAM_NUM_DEVICES:
    {
      cl_uint val;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_PROGRAM_CONTEXT:
    {
      cl_context val;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,sizeof(cl_context), &val, NULL))
      CHECK_ERR(::clRetainContext(val))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLContext, val));
      return;
    }
    case CL_PROGRAM_DEVICES:
    {
      size_t n=0;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,0,NULL, &n));
      n /= sizeof(cl_device_id);

      unique_ptr<cl_device_id[]> devices(new cl_device_id[n]);
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,sizeof(cl_device_id)*n, devices.get(), NULL));

      Local<Array> arr = Nan::New<Array>((int)n);
      for(uint32_t i=0;i<n;i++) {
        CHECK_ERR(::clRetainDevice(devices[i]))
        Nan::Set(arr, i, NOCL_WRAP(NoCLDeviceId, devices[i]));
      }

      info.GetReturnValue().Set(arr);
      return;
    }
    case CL_PROGRAM_BINARY_SIZES:
    {
      cl_uint nsizes;

      // TODO: Validate https://youtu.be/iQu9yG1dlrM?t=839
      // DRIVER ISSUE :  This part segfaults if program has not been compiled

      CHECK_ERR(::clGetProgramInfo(
        prog->getRaw(), CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint), &nsizes, NULL));

      unique_ptr<size_t[]> sizes(new size_t[nsizes]);

      CHECK_ERR(::clGetProgramInfo(
        prog->getRaw(), param_name, nsizes * sizeof(size_t), sizes.get(), NULL));

      Local<Array> arr = Nan::New<Array>(nsizes);
      for(cl_uint i=0;i<nsizes;i++)
        Nan::Set(arr, i,JS_INT(uint32_t(sizes[i])));

      info.GetReturnValue().Set(arr);
      return;
    }

    // TODO: Validate https://youtu.be/iQu9yG1dlrM?t=839
    /*
     err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t)*nb_devices, np, &nbread);//Load in np the size of my binary


    char** bn = new char* [nb_devices]; //Create the binary array


    for(int i =0; i < nb_devices;i++)  bn[i] = new char[np[i]]; // I know... it's bad... but if i use new char[np[i]], i have a segfault... :/


    err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(unsigned char *)*nb_devices, bn, &nbread); //Load the binary itself


     */
    case CL_PROGRAM_BINARIES:
    {
      cl_uint nsizes;

      // TODO: Validate https://youtu.be/iQu9yG1dlrM?t=839
      // DRIVER ISSUE :  This part segfaults if program has not been compiled

      CHECK_ERR(::clGetProgramInfo(
        prog->getRaw(), CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint), &nsizes, NULL));

      unique_ptr<size_t[]> sizes(new size_t[nsizes]);

      CHECK_ERR(::clGetProgramInfo(
        prog->getRaw(), CL_PROGRAM_BINARY_SIZES, nsizes * sizeof(size_t), sizes.get(), NULL));

      unsigned char** bn = new unsigned char* [nsizes];
      for(unsigned int i = 0; i < nsizes;i++)  {
        bn[i] = new unsigned char[sizes[i]];
      }

      CHECK_ERR(::clGetProgramInfo(
        prog->getRaw(), CL_PROGRAM_BINARIES, sizeof(unsigned char*)*nsizes, bn, sizes.get()));

      Local<Array> arr = Nan::New<Array>(nsizes);

      for (cl_uint i = 0; i < nsizes; ++ i) {
        Local<Object> bin = NOCL_WRAP(NoCLProgramBinary, bn[i]);
        const char* data = reinterpret_cast<const char*>(bn[i]);
        Local<Object> buf = Nan::CopyBuffer(data, sizes[i]).ToLocalChecked();
        Nan::Set(bin, JS_STR("buffer"), buf);
        Nan::Set(arr, i, bin);
      }

      info.GetReturnValue().Set(arr);
      return;
    }
#ifdef CL_VERSION_1_2
    case CL_PROGRAM_NUM_KERNELS:
    {
      size_t val;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,sizeof(size_t), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_PROGRAM_KERNEL_NAMES:
#endif
    case CL_PROGRAM_SOURCE:
    {
      size_t nchars;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(), param_name, 0, NULL, &nchars));
      unique_ptr<char[]> names(new char[nchars]);
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(), param_name, nchars*sizeof(char), names.get(), NULL));
      info.GetReturnValue().Set(JS_STR(names.get()));
      return;
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
  Nan::HandleScope scope;
  REQ_ARGS(3);

  NOCL_UNWRAP(prog, NoCLProgram, info[0]);
  NOCL_UNWRAP(device, NoCLDeviceId, info[1]);
  cl_program_build_info param_name = Nan::To<uint32_t>(info[2]).FromJust();

  switch(param_name) {
    case CL_PROGRAM_BUILD_STATUS:
    {
      cl_build_status val;
      CHECK_ERR(::clGetProgramBuildInfo(prog->getRaw(), device->getRaw(),param_name,sizeof(cl_build_status), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_PROGRAM_BUILD_OPTIONS:
    case CL_PROGRAM_BUILD_LOG:
    {
      size_t param_value_size_ret=0;
      CHECK_ERR(::clGetProgramBuildInfo(prog->getRaw(), device->getRaw(), param_name, 0, NULL, &param_value_size_ret));
      unique_ptr<char[]> param_value(new char[param_value_size_ret]);
      CHECK_ERR(::clGetProgramBuildInfo(prog->getRaw(), device->getRaw(), param_name, param_value_size_ret, param_value.get(), NULL));
      info.GetReturnValue().Set(JS_STR(param_value.get(),(int)param_value_size_ret));
      return;
    }
#ifdef CL_VERSION_1_2
    case CL_PROGRAM_BINARY_TYPE:
    {
      cl_program_binary_type val;
      CHECK_ERR(::clGetProgramBuildInfo(prog->getRaw(), device->getRaw(), param_name,sizeof(cl_program_binary_type), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
#endif
  }
  THROW_ERR(CL_INVALID_VALUE);
}

#ifdef CL_VERSION_2_1
// extern CL_API_ENTRY cl_program CL_API_CALL
// clCreateProgramWithIL(cl_context    /* context */,
//                      const void*    /* il */,
//                      size_t         /* length */,
//                      cl_int*        /* errcode_ret */) CL_API_SUFFIX__VERSION_2_1;
#endif

#ifdef CL_VERSION_2_2
// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetProgramReleaseCallback(cl_program          /* program */,
//                             void (CL_CALLBACK * /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
//                             void *              /* user_data */) CL_API_SUFFIX__VERSION_2_2;

// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetProgramSpecializationConstant(cl_program  /* program */,
//                                    cl_uint     /* spec_id */,
//                                    size_t      /* spec_size */,
//                                    const void* /* spec_value */) CL_API_SUFFIX__VERSION_2_2;
#endif

namespace Program {
NAN_MODULE_INIT(init)
{
  Nan::SetMethod(target, "createProgramWithSource", CreateProgramWithSource);
  Nan::SetMethod(target, "createProgramWithBinary", CreateProgramWithBinary);
#ifdef CL_VERSION_1_2
  Nan::SetMethod(target, "createProgramWithBuiltInKernels", CreateProgramWithBuiltInKernels);
#endif
  Nan::SetMethod(target, "retainProgram", RetainProgram);
  Nan::SetMethod(target, "releaseProgram", ReleaseProgram);
  Nan::SetMethod(target, "buildProgram", BuildProgram);
#ifdef CL_VERSION_1_2
  Nan::SetMethod(target, "compileProgram", CompileProgram);
  Nan::SetMethod(target, "linkProgram", LinkProgram);
  Nan::SetMethod(target, "unloadPlatformCompiler", UnloadPlatformCompiler);
#endif
  Nan::SetMethod(target, "unloadCompiler", UnloadPlatformCompiler);
  Nan::SetMethod(target, "getProgramInfo", GetProgramInfo);
  Nan::SetMethod(target, "getProgramBuildInfo", GetProgramBuildInfo);
#ifdef CL_VERSION_2_1
  // @TODO Nan::SetMethod(target, "createProgramWithIL", CreateProgramWithIL);
#endif
#ifdef CL_VERSION_2_2
  // @TODO Nan::SetMethod(target, "setProgramReleaseCallback", SetProgramReleaseCallback);
  // @TODO Nan::SetMethod(target, "setProgramSpecializationConstant", SetProgramSpecializationConstant);
#endif
}
} // namespace Program

} // namespace opencl
