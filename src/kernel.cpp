#include "kernel.h"
#include "types.h"

#include <unordered_map>
#include <functional>
#include <utility>
#include <tuple>

namespace opencl {

// /* Kernel Object APIs */
// extern CL_API_ENTRY cl_kernel CL_API_CALL
// clCreateKernel(cl_program      /* program */,
//                const char *    /* kernel_name */,
//                cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateKernel) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  NOCL_UNWRAP(program, NoCLProgram, info[0]);
  REQ_STR_ARG(1, name)

  cl_int ret=CL_SUCCESS;
  cl_kernel k = ::clCreateKernel(program->getRaw(), (const char*) *name, &ret);
  CHECK_ERR(ret);

  info.GetReturnValue().Set(NOCL_WRAP(NoCLKernel, k));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clCreateKernelsInProgram(cl_program     /* program */,
//                          cl_uint        /* num_kernels */,
//                          cl_kernel *    /* kernels */,
//                          cl_uint *      /* num_kernels_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateKernelsInProgram) {
  Nan::HandleScope scope;
  REQ_ARGS(1);


  // Arg 1 - Program
  NOCL_UNWRAP(program, NoCLProgram, info[0]);


  cl_uint numkernels;

  CHECK_ERR(::clCreateKernelsInProgram(program->getRaw(), 0, NULL, &numkernels));

  if (numkernels == 0) {
    THROW_ERR(CL_INVALID_VALUE);
  }

  cl_kernel * kernels = new cl_kernel[numkernels];
  CHECK_ERR(::clCreateKernelsInProgram(program->getRaw(), numkernels, kernels, NULL));

  Local<Array> karr = Nan::New<Array>();

  for(cl_uint i = 0; i < numkernels;i++) {
    // This should not be needed. clCreateKernelsInProgram implicitly does clRetainKernel
    // on each kernel
    // CHECK_ERR(::clRetainKernel(kernels[i]))
    Nan::Set(karr, i,NOCL_WRAP(NoCLKernel, kernels[i]));
  }

  delete[] kernels;

  info.GetReturnValue().Set(karr);
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainKernel(cl_kernel    /* kernel */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainKernel) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(k, NoCLKernel, info[0]);
  cl_int err=k->acquire();
  CHECK_ERR(err);
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseKernel(cl_kernel   /* kernel */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseKernel) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(k, NoCLKernel, info[0]);
  cl_int err=k->release();
  CHECK_ERR(err);
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

// caches OpenCL type name to conversion function mapping in a hash table
// (unordered_map) for fast retrieval. This is much faster than the previous
// approach of checking each possible type with strcmp in a huge if-else
class PrimitiveTypeMapCache {
private:
  /// Type of the conversion function
  typedef std::function<std::tuple<size_t, void*, cl_int>(const Local<Value>&)> func_t;
  // map of conversion functions
  std::unordered_map<std::string, func_t> m_converters;
public:
  PrimitiveTypeMapCache() {
    // if we create the TypeMap as a static function member, the constructor
    // is guarantueed to be called by only one thread (see C++11 standard sec 6.7)
    // while all other threads wait for completion. Thus no manual synchronization
    // is needed for the initialization.

    // create all type mappers

    /* convert primitive types */

    #define CONVERT_NUMBER(NAME, TYPE, PRED, CONV)                              \
     {                                                                          \
      func_t f = [](const Local<Value>& val)                                    \
        -> std::tuple<size_t, void*, cl_int> {                                  \
        if (!val->PRED()){                                                      \
         return std::tuple<size_t, void*,cl_int>(0, NULL, CL_INVALID_ARG_VALUE);\
        }                                                                       \
        void* ptr_data = new TYPE;                                              \
        size_t ptr_size = sizeof(TYPE);                                         \
        *((TYPE *)ptr_data) = (TYPE) Nan::To<CONV>(val).FromJust();            \
        return std::tuple<size_t, void*,cl_int>(ptr_size, ptr_data, 0);         \
      };                                                                        \
      m_converters[NAME] = f;                                                   \
     }

    CONVERT_NUMBER("char", cl_char, IsInt32, int32_t);
    CONVERT_NUMBER("uchar", cl_uchar, IsInt32, uint32_t);
    CONVERT_NUMBER("short", cl_short, IsInt32, int32_t);
    CONVERT_NUMBER("ushort", cl_ushort, IsInt32, uint32_t);
    CONVERT_NUMBER("int", cl_int , IsInt32, int32_t);
    CONVERT_NUMBER("uint", cl_uint, IsUint32, uint32_t);
    CONVERT_NUMBER("long", cl_long, IsNumber, int64_t);
    CONVERT_NUMBER("ulong", cl_ulong, IsNumber, int64_t);
    CONVERT_NUMBER("float", cl_float, IsNumber, double);
    CONVERT_NUMBER("double", cl_double, IsNumber, double);
    CONVERT_NUMBER("half", cl_half, IsNumber, double);

    #undef CONVERT_NUMBER

    /* convert vector types (e.g. float4, int16, etc) */

    #define CONVERT_VECT(NAME, TYPE, I, PRED, COND)                                     \
      {                                                                                 \
       func_t f = [](const Local<Value>& val)                                           \
          -> std::tuple<size_t, void*, cl_int> {                                        \
        if (!val->IsArray()) {                                                          \
          /*THROW_ERR(CL_INVALID_ARG_VALUE);  */                                        \
          return std::tuple<size_t,void*,cl_int>(0, NULL, CL_INVALID_ARG_VALUE);        \
        }                                                                               \
        Local<Array> arr = Local<Array>::Cast(val);                                     \
        if (arr->Length() != I) {                                                       \
          /*THROW_ERR(CL_INVALID_ARG_SIZE);*/                                           \
          return std::tuple<size_t,void*,cl_int>(0, NULL, CL_INVALID_ARG_SIZE);         \
        }                                                                               \
        TYPE * vvc = new TYPE[I];                                                       \
        size_t ptr_size = sizeof(TYPE) * I;                                             \
        void* ptr_data = vvc;                                                           \
        for (unsigned int i = 0; i < I; ++ i) {                                         \
          if (!Nan::Get(arr, i).ToLocalChecked()->PRED()) {                             \
            /*THROW_ERR(CL_INVALID_ARG_VALUE);*/                                        \
            /*THROW_ERR(CL_INVALID_ARG_VALUE);*/                                        \
          return std::tuple<size_t,void*,cl_int>(0, NULL, CL_INVALID_ARG_VALUE);        \
          }                                                                             \
          vvc[i] = (TYPE) Nan::To<COND>(Nan::Get(arr, i).ToLocalChecked()).FromJust(); \
        }                                                                               \
        return std::tuple<size_t,void*,cl_int>(ptr_size, ptr_data, 0);                  \
      };                                                                                \
      m_converters[NAME #I ] = f;                                                       \
      }

    #define CONVERT_VECTS(NAME, TYPE, PRED, COND) \
      CONVERT_VECT(NAME, TYPE, 2, PRED, COND);\
      CONVERT_VECT(NAME, TYPE, 3, PRED, COND);\
      CONVERT_VECT(NAME, TYPE, 4, PRED, COND);\
      CONVERT_VECT(NAME, TYPE, 8, PRED, COND);\
      CONVERT_VECT(NAME, TYPE, 16, PRED, COND);

    CONVERT_VECTS("char", cl_char, IsInt32, int32_t);
    CONVERT_VECTS("uchar", cl_uchar, IsInt32, int32_t);
    CONVERT_VECTS("short", cl_short, IsInt32, int32_t);
    CONVERT_VECTS("ushort", cl_ushort, IsInt32, int32_t);
    CONVERT_VECTS("int", cl_int, IsInt32, int32_t);
    CONVERT_VECTS("uint", cl_uint, IsUint32, uint32_t);
    CONVERT_VECTS("long", cl_long, IsNumber, int64_t);
    CONVERT_VECTS("ulong", cl_ulong, IsNumber, int64_t);
    CONVERT_VECTS("float", cl_float, IsNumber, double);
    CONVERT_VECTS("double", cl_double, IsNumber, double);
    CONVERT_VECTS("half", cl_half, IsNumber, double);

    #undef CONVERT_VECT
    #undef CONVERT_VECTS

    // add boolean conversion
    m_converters["bool"] = [](const Local<Value>& val) {
        size_t ptr_size = sizeof(cl_bool);
        void* ptr_data = new cl_bool;
        *((cl_bool *)ptr_data) = Nan::To<bool>(val).FromJust() ? 1 : 0;
        return std::tuple<size_t,void*,cl_int>(ptr_size, ptr_data, 0);
    };
  }

  /// Returns wheather the type given is in the map, i.e. if it is a
  /// primitive type
  bool hasType(const std::string& name){
      return m_converters.find(name) != m_converters.end();
  }

  // Converts the given JS value to the OpenCL type given by the `name`
  // parameter and returns the result as a pair of the size of the converted
  // value and the pointer as `void*`. The caller is responsible
  // for deleting the pointer after use.
  std::tuple<size_t, void*, cl_int> convert(const std::string& name, const Local<Value>& val) {
      assert(this->hasType(name));
      // call conversion function and return size of argument and pointer
      return m_converters[name](val);
  }

};

// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetKernelArg(cl_kernel    /* kernel */,
//                cl_uint      /* arg_index */,
//                size_t       /* arg_size */,
//                const void * /* arg_value */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(SetKernelArg) {
  // static member of the function gets initialized by the first thread
  // which calls this function. This is thread-safe according to the C++11 standard.
  // All other threads arriving wait till the constructor initialization is
  // complete before executing the code below.
  static PrimitiveTypeMapCache type_converter;

  Nan::HandleScope scope;
#ifdef CL_VERSION_1_2
  REQ_ARGS(3);
#else
  // if kernel introspection is not supported, require the type to be
  // passed as the 4th paramter, otherwise this parameter is optional
  REQ_ARGS(4);
#endif

  // Arg 0
  NOCL_UNWRAP(k, NoCLKernel, info[0]);

  // Arg 1
  unsigned int arg_idx = Nan::To<uint32_t>(info[1]).FromJust();

  // get type and qualifier of kernel parameter with this index
  // using OpenCL, and then try to convert arg[2] to the type the kernel
  // expects

  // check if we have kernel introspection available
  std::string type_name;
  bool local_arg = false;
#ifdef CL_VERSION_1_2
  // get address qualifier of kernel (local, global, constant, private), one of:
  // - CL_KERNEL_ARG_ADDRESS_GLOBAL
  // - CL_KERNEL_ARG_ADDRESS_LOCAL
  // - CL_KERNEL_ARG_ADDRESS_CONSTANT
  // - CL_KERNEL_ARG_ADDRESS_PRIVATE
  if(!ARG_EXISTS(2)) {
    cl_kernel_arg_address_qualifier adrqual;
    CHECK_ERR(::clGetKernelArgInfo(k->getRaw(), arg_idx, CL_KERNEL_ARG_ADDRESS_QUALIFIER, sizeof(cl_kernel_arg_address_qualifier), &adrqual, NULL));

    // get typename (for conversion of the JS parameter)
    size_t nchars=0;
    CHECK_ERR(::clGetKernelArgInfo(k->getRaw(), arg_idx, CL_KERNEL_ARG_TYPE_NAME, 0, NULL, &nchars));
    char* tname = new char[nchars];
    CHECK_ERR(::clGetKernelArgInfo(k->getRaw(), arg_idx, CL_KERNEL_ARG_TYPE_NAME, nchars, tname, NULL));
    type_name = std::string(tname);
    delete [] tname;
    if (adrqual == CL_KERNEL_ARG_ADDRESS_LOCAL)
      local_arg = true;
  } else
#endif
  { // behaviour when type is given
    // read argument 2 as the name of the data type
    if (info[2]->IsString()) {
      Local<String> s = Nan::To<String>(info[2]).ToLocalChecked();
      Nan::Utf8String tname(s);
      const char* tname_c = *tname;
      // cout<<"setKernelArg[3]="<<tname_c<<endl;
      size_t len = tname.length();
      type_name.resize(len);
      std::copy(tname_c, tname_c + len, type_name.begin());
      if (type_name == "local" || type_name == "__local")
        local_arg = true;
    } else {
      return Nan::ThrowError("Typename has to be given as string");
    }
  }


  cl_int err = 0;

  if (local_arg) {
    // expect a size type
    if (!info[3]->IsNumber())
      THROW_ERR(CL_INVALID_ARG_VALUE);
    // local buffers are intialized with their size (data = NULL)
    size_t local_size = Nan::To<int32_t>(info[3]).FromJust();
    err = ::clSetKernelArg(k->getRaw(), arg_idx, local_size, NULL);
  } else if ('*' == type_name[type_name.length() - 1] || type_name == "cl_mem"){
    // type must be a buffer (CLMem object)
    NOCL_UNWRAP(mem , NoCLMem, info[3]);
    const void *data = mem->getRaw();
    err = ::clSetKernelArg(k->getRaw(), arg_idx, sizeof(cl_mem), &data);
  } else if (type_converter.hasType(type_name)) {
    // convert primitive types using the conversion
    // function map (indexed by OpenCL type name)
    void* data;
    size_t size;
    std::tie(size, data, err) = type_converter.convert(type_name, info[3]);
    CHECK_ERR(err);
    err = ::clSetKernelArg(k->getRaw(), arg_idx, size, data);
    free(data);
  }

  // TODO: check for image_t types
  // TODO: support queue_t and clk_event_t, and others?

  // Otherwise it should be a native type
  else if (type_name == "sampler_t") {
    NOCL_UNWRAP(sw , NoCLSampler, info[3]);
    const void* data = k->getRaw();
    err = ::clSetKernelArg(k->getRaw(), arg_idx, sizeof(cl_sampler), &data);
  } else {
    std::string errstr = std::string("Unsupported OpenCL argument type: ") + type_name;
    return Nan::ThrowError(errstr.c_str());
  }

  CHECK_ERR(err);
  info.GetReturnValue().Set(JS_INT(err));
}


// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetKernelInfo(cl_kernel       /* kernel */,
//                 cl_kernel_info  /* param_name */,
//                 size_t          /* param_value_size */,
//                 void *          /* param_value */,
//                 size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetKernelInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  NOCL_UNWRAP(k, NoCLKernel, info[0]);
  cl_kernel_info param_name = Nan::To<uint32_t>(info[1]).FromJust();

  switch(param_name) {
#ifdef CL_VERSION_1_2
    case CL_KERNEL_ATTRIBUTES:
#endif
    case CL_KERNEL_FUNCTION_NAME: {
      size_t nchars=0;
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,0,NULL,&nchars));
      unique_ptr<char[]> name(new char[nchars]);
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,nchars,name.get(),NULL));
      info.GetReturnValue().Set(JS_STR(name.get()));
      return;
    }
    case CL_KERNEL_NUM_ARGS:
    case CL_KERNEL_REFERENCE_COUNT: {
      cl_uint num=0;
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,sizeof(cl_uint),&num, NULL));
      info.GetReturnValue().Set(JS_INT(num));
      return;
    }
    case CL_KERNEL_CONTEXT: {
      cl_context ctx=0;
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,sizeof(cl_context),&ctx, NULL));
      CHECK_ERR(::clRetainContext(ctx))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLContext, ctx));
      return;
    }
    case CL_KERNEL_PROGRAM: {
      cl_program p=0;
      CHECK_ERR(::clGetKernelInfo(k->getRaw(),param_name,sizeof(cl_program),&p, NULL));
      CHECK_ERR(::clRetainProgram(p))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLProgram, p));
      return;
    }
  }

  return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(CL_INVALID_VALUE)));
}

#ifdef CL_VERSION_1_2
// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetKernelArgInfo(cl_kernel       /* kernel */,
//                    cl_uint         /* arg_indx */,
//                    cl_kernel_arg_info  /* param_name */,
//                    size_t          /* param_value_size */,
//                    void *          /* param_value */,
//                    size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_2;
NAN_METHOD(GetKernelArgInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(3);

  NOCL_UNWRAP(k, NoCLKernel, info[0]);
  cl_uint arg_idx = Nan::To<uint32_t>(info[1]).FromJust();
  cl_kernel_arg_info param_name = Nan::To<uint32_t>(info[2]).FromJust();

  switch(param_name) {
    case CL_KERNEL_ARG_ADDRESS_QUALIFIER: {
      cl_kernel_arg_address_qualifier num=0;
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,sizeof(cl_kernel_arg_address_qualifier),&num, NULL));
      info.GetReturnValue().Set(JS_INT(num));
      return;
    }
    case CL_KERNEL_ARG_ACCESS_QUALIFIER: {
      cl_kernel_arg_access_qualifier num=0;
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,sizeof(cl_kernel_arg_access_qualifier),&num, NULL));
      info.GetReturnValue().Set(JS_INT(num));
      return;
    }
    case CL_KERNEL_ARG_TYPE_QUALIFIER: {
      cl_kernel_arg_type_qualifier num=0;
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,sizeof(cl_kernel_arg_type_qualifier),&num, NULL));
      info.GetReturnValue().Set(JS_INT(num));
      return;
    }
    case CL_KERNEL_ARG_TYPE_NAME:
    case CL_KERNEL_ARG_NAME: {
      size_t nchars=0;
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,0,NULL,&nchars));
      unique_ptr<char[]> name(new char[nchars]);
      CHECK_ERR(::clGetKernelArgInfo(k->getRaw(),arg_idx,param_name,nchars,name.get(),NULL));
      info.GetReturnValue().Set(JS_STR(name.get()));
      return;
    }
  }

  return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(CL_INVALID_VALUE)));
}
#endif

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetKernelWorkGroupInfo(cl_kernel                  /* kernel */,
//                          cl_device_id               /* device */,
//                          cl_kernel_work_group_info  /* param_name */,
//                          size_t                     /* param_value_size */,
//                          void *                     /* param_value */,
//                          size_t *                   /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetKernelWorkGroupInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(3);

  NOCL_UNWRAP(k, NoCLKernel, info[0]);
  NOCL_UNWRAP(d, NoCLDeviceId, info[1]);

  cl_kernel_work_group_info param_name = Nan::To<uint32_t>(info[2]).FromJust();

  switch(param_name) {
#ifdef CL_VERSION_1_2
    case CL_KERNEL_GLOBAL_WORK_SIZE:
#endif
    case CL_KERNEL_COMPILE_WORK_GROUP_SIZE: {
      size_t sz[3] = {0,0,0};
      CHECK_ERR(::clGetKernelWorkGroupInfo(k->getRaw(),d->getRaw(),param_name,3*sizeof(size_t),sz, NULL));
      Local<Array> szarr = Nan::New<Array>();
      Nan::Set(szarr, 0,JS_INT(sz[0]));
      Nan::Set(szarr, 1,JS_INT(sz[1]));
      Nan::Set(szarr, 2,JS_INT(sz[2]));
      info.GetReturnValue().Set(szarr);
      return;
    }
    case CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE:
    case CL_KERNEL_WORK_GROUP_SIZE: {
      size_t sz=0;
      CHECK_ERR(::clGetKernelWorkGroupInfo(k->getRaw(),d->getRaw(),param_name,sizeof(size_t),&sz, NULL));
      info.GetReturnValue().Set(JS_INT(sz));
      return;
    }
    case CL_KERNEL_LOCAL_MEM_SIZE:
    case CL_KERNEL_PRIVATE_MEM_SIZE: {
      cl_ulong sz=0;
      CHECK_ERR(::clGetKernelWorkGroupInfo(k->getRaw(),d->getRaw(),param_name,sizeof(cl_ulong),&sz, NULL));
      /**
        JS Compatibility

        As JS does not support 64 bits integer, therefore we return the values in kilobytes
      */
      info.GetReturnValue().Set(JS_INT((uint32_t) (sz >> 10)));
      return;
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

#ifdef CL_VERSION_2_1
// extern CL_API_ENTRY cl_kernel CL_API_CALL
// clCloneKernel(cl_kernel      source_kernel ,
//               cl_int*       /* errcode_ret */) CL_API_SUFFIX__VERSION_2_1;

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetKernelSubGroupInfo(cl_kernel                   /* kernel */,
//                         cl_device_id                /* device */,
//                         cl_kernel_sub_group_info    /* param_name */,
//                         size_t                      /* input_value_size */,
//                         const void*                 /*input_value */,
//                         size_t                      /* param_value_size */,
//                         void*                       /* param_value */,
//                         size_t*                     /* param_value_size_ret */ ) CL_API_SUFFIX__VERSION_2_1;
#endif

namespace Kernel {
NAN_MODULE_INIT(init)
{
  Nan::SetMethod(target, "createKernel", CreateKernel);
  Nan::SetMethod(target, "createKernelsInProgram", CreateKernelsInProgram);
  Nan::SetMethod(target, "retainKernel", RetainKernel);
  Nan::SetMethod(target, "releaseKernel", ReleaseKernel);
  Nan::SetMethod(target, "setKernelArg", SetKernelArg);
  Nan::SetMethod(target, "getKernelInfo", GetKernelInfo);
  Nan::SetMethod(target, "getKernelArgInfo", GetKernelArgInfo);
  Nan::SetMethod(target, "getKernelWorkGroupInfo", GetKernelWorkGroupInfo);
#ifdef CL_VERSION_2_0
  // @TODO Nan::SetMethod(target, "setKernelArgSVMPointer", SetKernelArgSVMPointer);
  // @TODO Nan::SetMethod(target, "setKernelExecInfo", SetKernelExecInfo);
#endif
#ifdef CL_VERSION_2_1
  // @TODO Nan::SetMethod(target, "cloneKernel", CloneKernel);
  // @TODO Nan::SetMethod(target, "getKernelSubGroupInfo", GetKernelSubGroupInfo);
#endif
}
} //namespace Kernel

} // namespace opencl
