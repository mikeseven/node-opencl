#include "program.h"
#include "types.h"
#include <vector>
#include "nanextension.h"

namespace opencl {

class NoCLProgramCLCallback:public NanAsyncLaunch {
 public:
   NoCLProgramCLCallback(NanCallback* callback,const v8::Local<v8::Object> &userData):NanAsyncLaunch(callback){
       NanScope();
       v8::Local<v8::Object> obj = NanNew<v8::Object>();
       NanAssignPersistent(persistentHandle, obj);
       v8::Local<v8::Object>  handle = NanNew(persistentHandle);
       handle->Set(kIndex, userData);
   }

   void CallBackIsDone(cl_program prg){
       this->program = prg;
       this->FireAndForget();
   }

   void Execute() {
     NanEscapableScope();
     v8::Local<v8::Object> handle = NanNew(persistentHandle);
     v8::Local<v8::Object> userData= (handle->Get(kIndex)).As<v8::Object>();
     Handle<Value> argv[] = {
         NanNew(NOCL_WRAP(NoCLProgram, program)),
         NanNew(userData)
     };
     callback->Call(2,argv);
   }

 private:
   cl_program program;

};

void CL_CALLBACK notifyPCB (cl_program program, void *user_data) {
    NoCLProgramCLCallback* asyncCB = static_cast<NoCLProgramCLCallback*>(user_data);
    if(asyncCB!=nullptr)
        asyncCB->CallBackIsDone(program);
}


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

  Local<Array> sizes = Local<Array>::Cast(args[2]);
  if (sizes->Length() == 0) {
    THROW_ERR(CL_INVALID_VALUE)
  }
  const size_t n = sizes->Length();
  unique_ptr<size_t[]> lengths(new size_t[n]);

  for (unsigned int i = 0; i < sizes->Length(); ++ i) {
    lengths[i] = sizes->Get(0)->Int32Value();
  }

  std::vector<NoCLProgramBinary> cl_binaries;
  std::vector<unsigned const char *> cl_binaries_str;

  REQ_ARRAY_ARG(3, js_binaries);
  NOCL_TO_ARRAY(cl_binaries, js_binaries, NoCLProgramBinary);

  if (sizes->Length() != js_binaries->Length()) {
    THROW_ERR(CL_INVALID_VALUE)
  }

  for (unsigned int i = 0; i < cl_binaries.size(); ++ i){
    cl_binaries_str.push_back(cl_binaries[i].getRaw());
  }

  cl_int ret=CL_SUCCESS;
  cl_program p=::clCreateProgramWithBinary(
    context->getRaw(),
    (cl_uint) cl_devices.size(),
    NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
    lengths.get(),
    &cl_binaries_str[0],
    NULL,
    &ret);

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
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 1
  vector<NoCLDeviceId> cl_devices;
  REQ_ARRAY_ARG(1, devices);
  NOCL_TO_ARRAY(cl_devices, devices, NoCLDeviceId);

  // Arg 2
  vector<char *> names;
  REQ_ARRAY_ARG(2, js_names);
  for (unsigned int i=0; i < js_names->Length(); ++ i) {
    if (!js_names->IsString()) {
      THROW_ERR(CL_INVALID_VALUE);
    }
    names.push_back(*String::Utf8Value(js_names->Get(i)));
  }

  if (names.size() == 0) {
    THROW_ERR(CL_INVALID_VALUE);
  }

  cl_int err = CL_SUCCESS;
  cl_program prg = ::clCreateProgramWithBuiltInKernels(
    context->getRaw(),
    cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
    names.front(),
    &err);

  CHECK_ERR(err);

  NanReturnValue(NOCL_WRAP(NoCLProgram, prg));
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
      THROW_ERR(CL_INVALID_COMPILER_OPTIONS)
    }
    options = new String::Utf8Value(args[2]);
  }

  //REQ_STR_ARG(2,options);

  //  callback + userdata
  int err;

  if (ARG_EXISTS(3)) {
   Local<Function> callbackHandle = args[3].As<Function>();
   NanCallback *callback = new NanCallback(callbackHandle);
   Local<Object> userData = args[4].As<Object>();
   NoCLProgramCLCallback* cb = new NoCLProgramCLCallback(callback,userData);

   err = ::clBuildProgram(p->getRaw(),
          devices.size(), NOCL_TO_CL_ARRAY(devices, NoCLDeviceId),
          options != NULL ? **options : nullptr,
          notifyPCB, cb);
  }


  else
    err = ::clBuildProgram(p->getRaw(),
          devices.size(), NOCL_TO_CL_ARRAY(devices, NoCLDeviceId),
          options != NULL ? **options : nullptr,
          nullptr, nullptr);

  delete options;


  CHECK_ERR(err);


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

  std::vector<NoCLDeviceId> cl_devices;

  // Arg 2 : devices
  if (ARG_EXISTS(1)) {
    REQ_ARRAY_ARG(1, js_devices);
    NOCL_TO_ARRAY(cl_devices, js_devices, NoCLDeviceId);
  }

  // Arg 3 : Options
  String::Utf8Value * options = nullptr;

  if (ARG_EXISTS(2)){
    if (!args[2]->IsString()) {
      THROW_ERR(CL_INVALID_COMPILER_OPTIONS)
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
    THROW_ERR(CL_INVALID_VALUE);
  }

  // CL CALL
  //  callback + userdata
  int err;

  if (ARG_EXISTS(5)) {
   Local<Function> callbackHandle = args[5].As<Function>();
   NanCallback *callback = new NanCallback(callbackHandle);
   Local<Object> userData = args[6].As<Object>();
   NoCLProgramCLCallback* cb = new NoCLProgramCLCallback(callback,userData);

   err = ::clCompileProgram(
               p->getRaw(),
               cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
               options != NULL ? **options : nullptr,
               program_headers.size(), NOCL_TO_CL_ARRAY(program_headers, NoCLProgram),
               &names.front(),
               notifyPCB, cb);
  }


  else
    err = ::clCompileProgram(
                p->getRaw(),
                cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
                options != NULL ? **options : nullptr,
                program_headers.size(), NOCL_TO_CL_ARRAY(program_headers, NoCLProgram),
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
  REQ_ARGS(4);


  //Arg 0
  NOCL_UNWRAP(ctx, NoCLContext, args[0]);

  std::vector<NoCLDeviceId> cl_devices;

  //Arg 1
  if (ARG_EXISTS(1)) {
    REQ_ARRAY_ARG(1, js_devices);
    NOCL_TO_ARRAY(cl_devices, js_devices, NoCLDeviceId);
  }

  //Arg 2
  String::Utf8Value * options = nullptr;

  if (ARG_EXISTS(2)){
    if (!args[2]->IsString()) {
      THROW_ERR(CL_INVALID_COMPILER_OPTIONS)
    }
    options = new String::Utf8Value(args[2]);
  }

  //Arg 3
  std::vector<NoCLProgram> cl_programs;

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
   Local<Function> callbackHandle = args[4].As<Function>();
   NanCallback *callback = new NanCallback(callbackHandle);
   Local<Object> userData = args[5].As<Object>();
   NoCLProgramCLCallback* cb = new NoCLProgramCLCallback(callback,userData);
   prg = ::clLinkProgram(
     ctx->getRaw(),
     cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
     options != NULL ? **options : nullptr,
     cl_programs.size(), NOCL_TO_CL_ARRAY(cl_programs, NoCLProgram),
     notifyPCB, cb,
     &ret);
  }

  else
    prg = ::clLinkProgram(
    ctx->getRaw(),
    cl_devices.size(), NOCL_TO_CL_ARRAY(cl_devices, NoCLDeviceId),
    options != NULL ? **options : nullptr,
    cl_programs.size(), NOCL_TO_CL_ARRAY(cl_programs, NoCLProgram),
    NULL, NULL,
    &ret);

  CHECK_ERR(ret);
  NanReturnValue(NOCL_WRAP(NoCLProgram, prg));
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

  NOCL_UNWRAP(prog, NoCLProgram, args[0]);

  cl_program_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_PROGRAM_REFERENCE_COUNT:
    case CL_PROGRAM_NUM_DEVICES:
    {
      cl_uint val;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_PROGRAM_CONTEXT:
    {
      cl_context val;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,sizeof(cl_context), &val, NULL))
      NanReturnValue(NOCL_WRAP(NoCLContext, val));
    }
    case CL_PROGRAM_DEVICES:
    {
      size_t n=0;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,0,NULL, &n));
      n /= sizeof(cl_device_id);

      unique_ptr<cl_device_id[]> devices(new cl_device_id[n]);
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,sizeof(cl_device_id)*n, devices.get(), NULL));

      Local<Array> arr = NanNew<Array>((int)n);
      for(uint32_t i=0;i<n;i++) {
        arr->Set(i, NOCL_WRAP(NoCLDeviceId, devices[i]));
      }

      NanReturnValue(arr);
    }
    case CL_PROGRAM_BINARY_SIZES:
    {
      cl_uint nsizes;

      // DRIVER ISSUE :  This part segfaults if program has not been compiled

      CHECK_ERR(::clGetProgramInfo(
        prog->getRaw(), CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint), &nsizes, NULL));

      unique_ptr<size_t[]> sizes(new size_t[nsizes]);

      CHECK_ERR(::clGetProgramInfo(
        prog->getRaw(), param_name, nsizes * sizeof(size_t), sizes.get(), NULL));

      Local<Array> arr = NanNew<Array>(nsizes);
      for(cl_uint i=0;i<nsizes;i++)
        arr->Set(i,JS_INT(sizes[i]));

      NanReturnValue(arr);
    }

    /*
     err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t)*nb_devices, np, &nbread);//Load in np the size of my binary


    char** bn = new char* [nb_devices]; //Create the binary array


    for(int i =0; i < nb_devices;i++)  bn[i] = new char[np[i]]; // I know... it's bad... but if i use new char[np[i]], i have a segfault... :/


    err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(unsigned char *)*nb_devices, bn, &nbread); //Load the binary itself


     */
    case CL_PROGRAM_BINARIES:
    {
      cl_uint nsizes;

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
        prog->getRaw(), param_name, sizeof(unsigned char*)*nsizes, bn, NULL));

      Local<Array> arr = NanNew<Array>(nsizes);

      for (cl_uint i = 0; i < nsizes; ++ i) {
        arr->Set(i, NOCL_WRAP(NoCLProgramBinary, bn[i]));
      }

      NanReturnValue(arr);
    }
    case CL_PROGRAM_NUM_KERNELS:
    {
      size_t val;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(),param_name,sizeof(size_t), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_PROGRAM_SOURCE:
    case CL_PROGRAM_KERNEL_NAMES:
    {
      size_t nchars;
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(), param_name, 0, NULL, &nchars));
      unique_ptr<char[]> names(new char[nchars]);
      CHECK_ERR(::clGetProgramInfo(prog->getRaw(), param_name, nchars*sizeof(char), names.get(), NULL));
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
