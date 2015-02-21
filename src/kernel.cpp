#include "kernel.h"
#include "types.h"

namespace opencl {

// /* Kernel Object APIs */
// extern CL_API_ENTRY cl_kernel CL_API_CALL
// clCreateKernel(cl_program      /* program */,
//                const char *    /* kernel_name */,
//                cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateKernel) {
  NanScope();
  REQ_ARGS(2);

  NOCL_UNWRAP(program, NoCLProgram, args[0]);
  REQ_STR_ARG(1, name)

  cl_int ret=CL_SUCCESS;
  cl_kernel k = ::clCreateKernel(program->getRaw(), (const char*) *name, &ret);
  CHECK_ERR(ret);

  NanReturnValue(NOCL_WRAP(NoCLKernel, k));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clCreateKernelsInProgram(cl_program     /* program */,
//                          cl_uint        /* num_kernels */,
//                          cl_kernel *    /* kernels */,
//                          cl_uint *      /* num_kernels_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateKernelsInProgram) {
  NanScope();
  REQ_ARGS(1);

  // Arg 1 - Program
  NOCL_UNWRAP(program, NoCLProgram, args[0]);


  cl_uint numkernels;
  CHECK_ERR(::clCreateKernelsInProgram(program->getRaw(), NULL, NULL, &numkernels));

  if (numkernels == 0) {
    THROW_ERR(CL_INVALID_VALUE);
  }

  cl_kernel * kernels = new cl_kernel[numkernels];
  CHECK_ERR(::clCreateKernelsInProgram(program->getRaw(), numkernels, kernels, NULL));

  Local<Array> karr = NanNew<Array>();

  for(cl_uint i = 0; i < numkernels;i++) {
    karr->Set(i,NOCL_WRAP(NoCLKernel, kernels[i]));
  }

  delete kernels;

  NanReturnValue(karr);
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainKernel(cl_kernel    /* kernel */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainKernel) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(k, NoCLKernel, args[0]);
  cl_int count=clRetainKernel(k->getRaw());

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseKernel(cl_kernel   /* kernel */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseKernel) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(k, NoCLKernel, args[0]);
  cl_int count=clRetainKernel(k->getRaw());

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetKernelArg(cl_kernel    /* kernel */,
//                cl_uint      /* arg_index */,
//                size_t       /* arg_size */,
//                const void * /* arg_value */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(SetKernelArg) {
  NanScope();
  REQ_ARGS(4);

  // Arg 0
  NOCL_UNWRAP(k, NoCLKernel, args[0]);

  // Arg 1
  unsigned int idx = args[1]->Uint32Value();

  // Arg 2
  String::Utf8Value name(args[2]);

  // Arg 3
  size_t ptr_size = 0;
  void * ptr_data = NULL;

  // Boolean
  if (strcmp(*name, "bool") == 0) {
    ptr_size = sizeof(cl_bool);
    ptr_data = new cl_bool;

    *((cl_bool *)ptr_data) = args[3]->BooleanValue() ? 1 : 0;
  }

  #define CONVERT_NUMBER(NAME, TYPE, PRED, CONV) \
    if (strcmp(*name, NAME) == 0) { \
      if (!args[3]->PRED()){\
        THROW_ERR(CL_INVALID_ARG_VALUE) \
      }\
      ptr_data = new TYPE;\
      ptr_size = sizeof(TYPE); \
      *((TYPE *)ptr_data) = args[3]->CONV();\
    }

  CONVERT_NUMBER("char", cl_short, IsInt32, ToInt32()->Value);
  CONVERT_NUMBER("uchar", cl_short, IsInt32, ToInt32()->Value);
  CONVERT_NUMBER("short", cl_short, IsInt32, ToInt32()->Value);
  CONVERT_NUMBER("ushort", cl_ushort, IsInt32, ToInt32()->Value);
  CONVERT_NUMBER("int", cl_int , IsInt32, ToInt32()->Value);
  CONVERT_NUMBER("uint", cl_uint, IsInt32, ToInt32()->Value);
  CONVERT_NUMBER("long", cl_long, IsInt32, ToInt32()->Value);
  CONVERT_NUMBER("ulong", cl_ulong, IsInt32, ToInt32()->Value);
  CONVERT_NUMBER("float", cl_float, IsNumber, NumberValue);
  CONVERT_NUMBER("double", cl_double, IsNumber, NumberValue);
  CONVERT_NUMBER("half", cl_half, IsNumber, NumberValue);



  #define CONVERT_VECT(NAME, TYPE, I, PRED, COND) \
    if (strcmp(*name, "NAME ## I") == 0) { \
      if (!args[3]->IsArray()) {\
        THROW_ERR(CL_INVALID_ARG_VALUE);\
      }\
      Local<Array> arr = Local<Array>::Cast(args[3]);\
      if (arr->Length() != I) {\
        THROW_ERR(CL_INVALID_ARG_SIZE);\
      }\
      TYPE * vvc = new TYPE[I];\
      ptr_size = sizeof(TYPE) * I;\
      ptr_data = vvc;\
      for (unsigned int i = 0; i < I; ++ i) {\
        if (!arr->Get(i)->PRED()) {\
          THROW_ERR(CL_INVALID_ARG_VALUE);\
        }\
        vvc[i] = arr->Get(i)->COND();\
      }\
    }

  #define CONVERT_VECTS(NAME, TYPE, PRED, COND) \
    CONVERT_VECT(NAME, TYPE, 2, PRED, COND);\
    CONVERT_VECT(NAME, TYPE, 3, PRED, COND);\
    CONVERT_VECT(NAME, TYPE, 4, PRED, COND);\
    CONVERT_VECT(NAME, TYPE, 8, PRED, COND);\
    CONVERT_VECT(MAME, TYPE, 16, PRED, COND);

  CONVERT_VECTS("char", cl_char, IsInt32, ToInt32()->Value);
  CONVERT_VECTS("uchar", cl_uchar, IsInt32, ToInt32()->Value);
  CONVERT_VECTS("short", cl_short, IsInt32, ToInt32()->Value);
  CONVERT_VECTS("ushort", cl_ushort, IsInt32, ToInt32()->Value);
  CONVERT_VECTS("int", cl_int, IsInt32, ToInt32()->Value);
  CONVERT_VECTS("uint", cl_uint, IsInt32, ToInt32()->Value);
  CONVERT_VECTS("long", cl_long, IsInt32, ToInt32()->Value);
  CONVERT_VECTS("ulong", cl_ulong, IsInt32, ToInt32()->Value);
  CONVERT_VECTS("float", cl_char, IsNumber, NumberValue);
  CONVERT_VECTS("double", cl_char, IsNumber, NumberValue);

  bool dont_delete = false;

  // Otherwise it should be a native type
  if (strcmp(*name, "sampler_t") == 0) {
    NOCL_UNWRAP(sw , NoCLSampler, args[3]);
    ptr_data = sw->getRaw();
    ptr_size = sizeof(cl_sampler);
    dont_delete = true;
  }

  // And if it ends with a star it is a memobject
  if ('*' == (*name)[(strlen(*name) - 1)]){
    NOCL_UNWRAP(mem , NoCLMem, args[3]);
    ptr_data = mem->getRaw();
    ptr_size = sizeof(cl_mem);
    dont_delete = true;
  }

  if (ptr_data == NULL) {
    THROW_ERR(CL_INVALID_VALUE);
  }

  cl_int err = ::clSetKernelArg(
    k->getRaw(), idx, ptr_size, &ptr_data);

  if (!dont_delete) {
    free(ptr_data);
  }

  CHECK_ERR(err);

  NanReturnValue(JS_INT(err));

}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetKernelInfo(cl_kernel       /* kernel */,
//                 cl_kernel_info  /* param_name */,
//                 size_t          /* param_value_size */,
//                 void *          /* param_value */,
//                 size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetKernelInfo) {
  NanScope();
  REQ_ARGS(2);

  NOCL_UNWRAP(k, NoCLKernel, args[0]);
  cl_kernel_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_KERNEL_ATTRIBUTES:
    case CL_KERNEL_FUNCTION_NAME: {
      size_t nchars=0;
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,0,NULL,&nchars));
      unique_ptr<char[]> name(new char[nchars]);
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,nchars,name.get(),NULL));
      NanReturnValue(JS_STR(name.get()));
    }
    case CL_KERNEL_NUM_ARGS:
    case CL_KERNEL_REFERENCE_COUNT: {
      cl_uint num=0;
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,sizeof(cl_uint),&num, NULL));
      NanReturnValue(JS_INT(num));
    }
    case CL_KERNEL_CONTEXT: {
      cl_context ctx=0;
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,sizeof(cl_context),&ctx, NULL));
      NanReturnValue(NOCL_WRAP(NoCLContext, ctx));
    }
    case CL_KERNEL_PROGRAM: {
      cl_program p=0;
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,sizeof(cl_program),&p, NULL));
      NanReturnValue(NOCL_WRAP(NoCLProgram, p));
    }
  }

  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetKernelArgInfo(cl_kernel       /* kernel */,
//                    cl_uint         /* arg_indx */,
//                    cl_kernel_arg_info  /* param_name */,
//                    size_t          /* param_value_size */,
//                    void *          /* param_value */,
//                    size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(GetKernelArgInfo) {
  NanScope();
  REQ_ARGS(3);

  NOCL_UNWRAP(k, NoCLKernel, args[0]);
  cl_uint arg_idx = args[1]->Uint32Value();
  cl_kernel_arg_info param_name = args[2]->Uint32Value();

  switch(param_name) {
    case CL_KERNEL_ARG_ADDRESS_QUALIFIER: {
      cl_kernel_arg_address_qualifier num=0;
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,sizeof(cl_kernel_arg_address_qualifier),&num, NULL));
      NanReturnValue(JS_INT(num));
    }
    case CL_KERNEL_ARG_ACCESS_QUALIFIER: {
      cl_kernel_arg_access_qualifier num=0;
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,sizeof(cl_kernel_arg_access_qualifier),&num, NULL));
      NanReturnValue(JS_INT(num));
    }
    case CL_KERNEL_ARG_TYPE_QUALIFIER: {
      cl_kernel_arg_type_qualifier num=0;
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,sizeof(cl_kernel_arg_type_qualifier),&num, NULL));
      NanReturnValue(JS_INT(num));
    }
    case CL_KERNEL_ARG_TYPE_NAME:
    case CL_KERNEL_ARG_NAME: {
      size_t nchars=0;
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,0,NULL,&nchars));
      unique_ptr<char[]> name(new char[nchars]);
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,nchars,name.get(),NULL));
      NanReturnValue(JS_STR(name.get()));
    }
  }

  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetKernelWorkGroupInfo(cl_kernel                  /* kernel */,
//                          cl_device_id               /* device */,
//                          cl_kernel_work_group_info  /* param_name */,
//                          size_t                     /* param_value_size */,
//                          void *                     /* param_value */,
//                          size_t *                   /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetKernelWorkGroupInfo) {
  NanScope();
  REQ_ARGS(3);

  NOCL_UNWRAP(k, NoCLKernel, args[0]);
  NOCL_UNWRAP(d, NoCLDeviceId, args[1]);

  cl_kernel_work_group_info param_name = args[2]->Uint32Value();

  switch(param_name) {
    case CL_KERNEL_COMPILE_WORK_GROUP_SIZE:
    case CL_KERNEL_GLOBAL_WORK_SIZE: {
      size_t sz[3] = {0,0,0};
      CHECK_ERR(::clGetKernelWorkGroupInfo(k->getRaw(),d->getRaw(),param_name,3*sizeof(size_t),sz, NULL));
      Local<Array> szarr = NanNew<Array>();
      szarr->Set(0,JS_INT(sz[0]));
      szarr->Set(1,JS_INT(sz[1]));
      szarr->Set(2,JS_INT(sz[2]));
      NanReturnValue(szarr);
    }
    case CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE:
    case CL_KERNEL_WORK_GROUP_SIZE: {
      size_t sz=0;
      CHECK_ERR(::clGetKernelWorkGroupInfo(k->getRaw(),d->getRaw(),param_name,sizeof(size_t),&sz, NULL));
      NanReturnValue(JS_INT(sz));
    }
    case CL_KERNEL_LOCAL_MEM_SIZE:
    case CL_KERNEL_PRIVATE_MEM_SIZE: {
      cl_ulong sz=0;
      CHECK_ERR(::clGetKernelWorkGroupInfo(k->getRaw(),d->getRaw(),param_name,sizeof(cl_ulong),&sz, NULL));
      NanReturnValue(JS_INT(sz));
    }
  }

  THROW_ERR(CL_INVALID_VALUE);
}

#ifdef CL_VERSION_2_0
// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetKernelArgSVMPointer(cl_kernel    /* kernel */,
//                          cl_uint      /* arg_index */,
//                          const void * /* arg_value */) CL_API_SUFFIX__VERSION_2_0;

// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetKernelExecInfo(cl_kernel            /* kernel */,
//                     cl_kernel_exec_info  /* param_name */,
//                     size_t               /* param_value_size */,
//                     const void *         /* param_value */) CL_API_SUFFIX__VERSION_2_0;
#endif

namespace Kernel {
void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "createKernel", CreateKernel);
  NODE_SET_METHOD(exports, "createKernelsInProgram", CreateKernelsInProgram);
  NODE_SET_METHOD(exports, "retainKernel", RetainKernel);
  NODE_SET_METHOD(exports, "releaseKernel", ReleaseKernel);
  NODE_SET_METHOD(exports, "setKernelArg", SetKernelArg);
  NODE_SET_METHOD(exports, "getKernelInfo", GetKernelInfo);
  NODE_SET_METHOD(exports, "getKernelArgInfo", GetKernelArgInfo);
  NODE_SET_METHOD(exports, "getKernelWorkGroupInfo", GetKernelWorkGroupInfo);
}
} //namespace Kernel

} // namespace opencl
