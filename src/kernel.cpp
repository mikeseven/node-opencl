#include "kernel.h"

namespace opencl {

// /* Kernel Object APIs */
// extern CL_API_ENTRY cl_kernel CL_API_CALL
// clCreateKernel(cl_program      /* program */,
//                const char *    /* kernel_name */,
//                cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateKernel) {
  NanScope();
  REQ_ARGS(2);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_PROGRAM));
  }
  cl_program prog=Unwrap<cl_program>(args[0]);
  REQ_STR_ARG(1, name)

  cl_int ret=CL_SUCCESS;
  cl_kernel k = ::clCreateKernel(prog, (const char*) *name, &ret);
  CHECK_ERR(ret);

  NanReturnValue(Wrap(k));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clCreateKernelsInProgram(cl_program     /* program */,
//                          cl_uint        /* num_kernels */,
//                          cl_kernel *    /* kernels */,
//                          cl_uint *      /* num_kernels_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateKernelsInProgram) {
  NanScope();
  REQ_ARGS(2);

  // Arg 1 - Program

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_PROGRAM));
  }
  cl_program prog=Unwrap<cl_program>(args[0]);

  // Arg 2 - Number of kernels

  cl_uint nkernels = 0;
  if (args[1]->IsUint32()) {
    nkernels = args[1]->Uint32Value();
  } else {
    throwTypeMismatch(1, "number of kernels", "unsigned int");
  }


  unique_ptr<cl_kernel[]> kernels(new cl_kernel[nkernels]);

  CHECK_ERR(::clCreateKernelsInProgram(prog, nkernels, kernels.get(), NULL));

  Local<Array> karr = Array::New();
  for(cl_uint i=0;i<nkernels;i++) {
    karr->Set(i,Wrap<cl_kernel>(kernels[i]));
  }

  NanReturnValue(karr);
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainKernel(cl_kernel    /* kernel */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainKernel) {
  NanScope();
  REQ_ARGS(1);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_KERNEL));
  }

  cl_kernel k = Unwrap<cl_kernel>(args[0]);
  cl_int count=clRetainKernel(k);

  NanReturnValue(JS_INT(count));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseKernel(cl_kernel   /* kernel */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseKernel) {
  NanScope();
  REQ_ARGS(1);

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_KERNEL));
  }

  cl_kernel k = Unwrap<cl_kernel>(args[0]);
  cl_int count=clRetainKernel(k);

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
  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_KERNEL));
  }

  // cl_kernel k = Unwrap<cl_kernel>(args[0]);
  // cl_uint arg_index=args[1]->Uint32Value();
  // size_t arg_size=args[2]->Uint32Value();
  // TODO arg_value

  // NanReturnValue(JS_INT(CL_SUCCESS));
  NanReturnUndefined();
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

  if(!isOpenCLObj(args[0])) {
    return NanThrowError(JS_INT(CL_INVALID_KERNEL));
  }
  cl_kernel k=Unwrap<cl_kernel>(args[0]);
  cl_kernel_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_KERNEL_ATTRIBUTES:
    case CL_KERNEL_FUNCTION_NAME: {
      size_t nchars=0;
      CHECK_ERR(::clGetKernelInfo(k,param_name,0,NULL,&nchars));
      unique_ptr<char[]> name(new char[nchars]);
      CHECK_ERR(::clGetKernelInfo(k,param_name,nchars,name.get(),NULL));
      NanReturnValue(JS_STR(name.get()));
    }
    case CL_KERNEL_NUM_ARGS:
    case CL_KERNEL_REFERENCE_COUNT: {
      cl_uint num=0;
      CHECK_ERR(::clGetKernelInfo(k,param_name,sizeof(cl_uint),&num, NULL));
      NanReturnValue(JS_INT(num));
    }
    case CL_KERNEL_CONTEXT: {
      cl_context ctx=0;
      CHECK_ERR(::clGetKernelInfo(k,param_name,sizeof(cl_context),&ctx, NULL));
      NanReturnValue(Wrap(ctx));
    }
    case CL_KERNEL_PROGRAM: {
      cl_program p=0;
      CHECK_ERR(::clGetKernelInfo(k,param_name,sizeof(cl_program),&p, NULL));
      NanReturnValue(Wrap(p));
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

  if(!isOpenCLObj(args[0])) {
    THROW_ERR(CL_INVALID_KERNEL);
  }
  cl_kernel k=Unwrap<cl_kernel>(args[0]);
  cl_uint arg_idx = args[1]->Uint32Value();
  cl_kernel_arg_info param_name = args[2]->Uint32Value();

  switch(param_name) {
    case CL_KERNEL_ARG_ADDRESS_QUALIFIER: {
      cl_kernel_arg_address_qualifier num=0;
      CHECK_ERR(::clGetKernelArgInfo(k,arg_idx,param_name,sizeof(cl_kernel_arg_address_qualifier),&num, NULL));
      NanReturnValue(JS_INT(num));
    }
    case CL_KERNEL_ARG_ACCESS_QUALIFIER: {
      cl_kernel_arg_access_qualifier num=0;
      CHECK_ERR(::clGetKernelArgInfo(k,arg_idx,param_name,sizeof(cl_kernel_arg_access_qualifier),&num, NULL));
      NanReturnValue(JS_INT(num));
    }
    case CL_KERNEL_ARG_TYPE_QUALIFIER: {
      cl_kernel_arg_type_qualifier num=0;
      CHECK_ERR(::clGetKernelArgInfo(k,arg_idx,param_name,sizeof(cl_kernel_arg_type_qualifier),&num, NULL));
      NanReturnValue(JS_INT(num));
    }
    case CL_KERNEL_ARG_TYPE_NAME:
    case CL_KERNEL_ARG_NAME: {
      size_t nchars=0;
      CHECK_ERR(::clGetKernelArgInfo(k,arg_idx,param_name,0,NULL,&nchars));
      unique_ptr<char[]> name(new char[nchars]);
      CHECK_ERR(::clGetKernelArgInfo(k,arg_idx,param_name,nchars,name.get(),NULL));
      NanReturnValue(JS_STR(name.get()));
    }
  }

  THROW_ERR(CL_INVALID_VALUE);
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

  if(!isOpenCLObj(args[0])) {
    THROW_ERR(CL_INVALID_KERNEL);
  }
  cl_kernel k=Unwrap<cl_kernel>(args[0]);
  if(!isOpenCLObj(args[1])) {
    THROW_ERR(CL_INVALID_DEVICE);
  }
  cl_device_id d=Unwrap<cl_device_id>(args[1]);
  cl_kernel_work_group_info param_name = args[2]->Uint32Value();

  switch(param_name) {
    case CL_KERNEL_COMPILE_WORK_GROUP_SIZE:
    case CL_KERNEL_GLOBAL_WORK_SIZE: {
      size_t sz[3] = {0,0,0};
      CHECK_ERR(::clGetKernelWorkGroupInfo(k,d,param_name,3*sizeof(size_t),sz, NULL));
      Local<Array> szarr = Array::New();
      szarr->Set(0,JS_INT(sz[0]));
      szarr->Set(1,JS_INT(sz[1]));
      szarr->Set(2,JS_INT(sz[2]));
      NanReturnValue(szarr);
    }
    case CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE:
    case CL_KERNEL_WORK_GROUP_SIZE: {
      size_t sz=0;
      CHECK_ERR(::clGetKernelWorkGroupInfo(k,d,param_name,sizeof(size_t),&sz, NULL));
      NanReturnValue(JS_INT(sz));
    }
    case CL_KERNEL_LOCAL_MEM_SIZE:
    case CL_KERNEL_PRIVATE_MEM_SIZE: {
      cl_ulong sz=0;
      CHECK_ERR(::clGetKernelWorkGroupInfo(k,d,param_name,sizeof(cl_ulong),&sz, NULL));
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
