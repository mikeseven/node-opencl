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

  CHECK_ERR(::clCreateKernelsInProgram(program->getRaw(), 0, NULL, &numkernels));

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
  cl_int err=k->acquire();
  CHECK_ERR(err);
  NanReturnValue(JS_INT(CL_SUCCESS));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseKernel(cl_kernel   /* kernel */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseKernel) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(k, NoCLKernel, args[0]);
  cl_int err=k->release();
  CHECK_ERR(err);
  NanReturnValue(JS_INT(CL_SUCCESS));
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
        *((TYPE *)ptr_data) = val->CONV();                                      \
        return std::tuple<size_t, void*,cl_int>(ptr_size, ptr_data, 0);         \
      };                                                                        \
      m_converters[NAME] = f;                                                   \
     }

    CONVERT_NUMBER("char", cl_char, IsInt32, ToInt32()->Value);
    CONVERT_NUMBER("uchar", cl_uchar, IsInt32, ToUint32()->Value);
    CONVERT_NUMBER("short", cl_short, IsInt32, ToInt32()->Value);
    CONVERT_NUMBER("ushort", cl_ushort, IsInt32, ToUint32()->Value);
    CONVERT_NUMBER("int", cl_int , IsInt32, ToInt32()->Value);
    CONVERT_NUMBER("uint", cl_uint, IsInt32, ToUint32()->Value);
    CONVERT_NUMBER("long", cl_long, IsNumber, ToInteger()->Value);
    CONVERT_NUMBER("ulong", cl_ulong, IsNumber, ToInteger()->Value);
    CONVERT_NUMBER("float", cl_float, IsNumber, NumberValue);
    CONVERT_NUMBER("double", cl_double, IsNumber, NumberValue);
    CONVERT_NUMBER("half", cl_half, IsNumber, NumberValue);

    #undef CONVERT_NUMBER


    /* convert vector types (e.g. float4, int16, etc) */

    #define CONVERT_VECT(NAME, TYPE, I, PRED, COND)                             \
      {                                                                         \
       func_t f = [](const Local<Value>& val)                                   \
          -> std::tuple<size_t, void*, cl_int> {                                \
        if (!val->IsArray()) {                                                  \
          /*THROW_ERR(CL_INVALID_ARG_VALUE);  */                                \
          return std::tuple<size_t,void*,cl_int>(0, NULL, CL_INVALID_ARG_VALUE);\
        }                                                                       \
        Local<Array> arr = Local<Array>::Cast(val);                             \
        if (arr->Length() != I) {                                               \
          /*THROW_ERR(CL_INVALID_ARG_SIZE);*/                                   \
          return std::tuple<size_t,void*,cl_int>(0, NULL, CL_INVALID_ARG_SIZE); \
        }                                                                       \
        TYPE * vvc = new TYPE[I];                                               \
        size_t ptr_size = sizeof(TYPE) * I;                                     \
        void* ptr_data = vvc;                                                   \
        for (unsigned int i = 0; i < I; ++ i) {                                 \
          if (!arr->Get(i)->PRED()) {                                           \
            /*THROW_ERR(CL_INVALID_ARG_VALUE);*/                                \
            /*THROW_ERR(CL_INVALID_ARG_VALUE);*/                                \
          return std::tuple<size_t,void*,cl_int>(0, NULL, CL_INVALID_ARG_VALUE);\
          }                                                                     \
          vvc[i] = arr->Get(i)->COND();                                         \
        }                                                                       \
        return std::tuple<size_t,void*,cl_int>(ptr_size, ptr_data, 0);          \
      };                                                                        \
      m_converters["NAME ## I"] = f;                                            \
      }

    #define CONVERT_VECTS(NAME, TYPE, PRED, COND) \
      CONVERT_VECT(NAME, TYPE, 2, PRED, COND);\
      CONVERT_VECT(NAME, TYPE, 3, PRED, COND);\
      CONVERT_VECT(NAME, TYPE, 4, PRED, COND);\
      CONVERT_VECT(NAME, TYPE, 8, PRED, COND);\
      CONVERT_VECT(MAME, TYPE, 16, PRED, COND);

    CONVERT_VECTS("char", cl_char, IsInt32, ToInt32()->Value);
    CONVERT_VECTS("uchar", cl_uchar, IsInt32, ToUint32()->Value);
    CONVERT_VECTS("short", cl_short, IsInt32, ToInt32()->Value);
    CONVERT_VECTS("ushort", cl_ushort, IsInt32, ToUint32()->Value);
    CONVERT_VECTS("int", cl_int, IsInt32, ToInt32()->Value);
    CONVERT_VECTS("uint", cl_uint, IsInt32, ToUint32()->Value);
    CONVERT_VECTS("long", cl_long, IsNumber, ToInteger()->Value);
    CONVERT_VECTS("ulong", cl_ulong, IsNumber, ToInteger()->Value);
    CONVERT_VECTS("float", cl_float, IsNumber, NumberValue);
    CONVERT_VECTS("double", cl_double, IsNumber, NumberValue);
    CONVERT_VECTS("half", cl_half, IsNumber, NumberValue);

    #undef CONVERT_VECT
    #undef CONVERT_VECTS

    // add boolean conversion
    m_converters["bool"] = [](const Local<Value>& val) {
        size_t ptr_size = sizeof(cl_bool);
        void* ptr_data = new cl_bool;
        *((cl_bool *)ptr_data) = val->BooleanValue() ? 1 : 0;
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
      return std::move(m_converters[name](val));
  }

};

// extern CL_API_ENTRY cl_int CL_API_CALL
// clSetKernelArg(cl_kernel    /* kernel */,
//                cl_uint      /* arg_index */,
//                size_t       /* arg_size */,
//                const void * /* arg_value */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(SetKernelArg) {
  // static member of the function gets initialzed by the first thread
  // which calls this function. This is thread-safe due to the C++11 standard.
  // All other threads arriving wait till the constructor initialization is
  // complete before executing the code below.
  static PrimitiveTypeMapCache type_converter;

  NanScope();
  REQ_ARGS(3);

  // Arg 0
  NOCL_UNWRAP(k, NoCLKernel, args[0]);

  // Arg 1
  unsigned int arg_idx = args[1]->Uint32Value();

  // get type and qualifier of kernel parameter with this index
  // using OpenCL, and then try to convert arg[2] to the type the kernel
  // expects

  // get address qualifier of kernel (local, global, constant, private), one of:
  // - CL_KERNEL_ARG_ADDRESS_GLOBAL
  // - CL_KERNEL_ARG_ADDRESS_LOCAL
  // - CL_KERNEL_ARG_ADDRESS_CONSTANT
  // - CL_KERNEL_ARG_ADDRESS_PRIVATE
  cl_kernel_arg_address_qualifier adrqual;
  CHECK_ERR(::clGetKernelArgInfo(k->getRaw(), arg_idx, CL_KERNEL_ARG_ADDRESS_QUALIFIER, sizeof(cl_kernel_arg_address_qualifier), &adrqual, NULL));

  // get typename (for conversion of the JS parameter)
  size_t nchars=0;
  CHECK_ERR(::clGetKernelArgInfo(k->getRaw(), arg_idx, CL_KERNEL_ARG_TYPE_NAME, 0, NULL, &nchars));
  unique_ptr<char[]> type_name(new char[nchars]);
  CHECK_ERR(::clGetKernelArgInfo(k->getRaw(), arg_idx, CL_KERNEL_ARG_TYPE_NAME, nchars, type_name.get(), NULL));


  // now map the JS parameter `args[2]` to the expected kernel parameter type `typename`

  const char * name = type_name.get();
  cl_int err = 0;

  // first check for pointers (require either local size or cl_mem)
  if ('*' == name[(strlen(name) - 1)]){
    // check if type is local or global
    switch (adrqual) {
      case CL_KERNEL_ARG_ADDRESS_LOCAL:
        {
            // expect a size type
            if (!args[2]->IsNumber())
                THROW_ERR(CL_INVALID_ARG_VALUE);
            // local buffers are intialized with their size (data = NULL)
            size_t local_size = args[2]->ToInteger()->Value();
            err = ::clSetKernelArg(k->getRaw(), arg_idx, local_size, NULL);
        } break;
      case CL_KERNEL_ARG_ADDRESS_GLOBAL:
      case CL_KERNEL_ARG_ADDRESS_CONSTANT:
        {
          // global and constant memory parameters have to be initialized with
          // a cl_mem buffer reference
          NOCL_UNWRAP(mem , NoCLMem, args[2]);
          err = ::clSetKernelArg(k->getRaw(), arg_idx, sizeof(cl_mem), &mem->getRaw());
        } break;
    }
  } else if (type_converter.hasType(name)) {
    // convert primitive types using the conversion
    // function map (indexed by OpenCL type name)
    void* data;
    size_t size;
    std::tie(size, data, err) = type_converter.convert(name, args[2]);
    CHECK_ERR(err);
    err = ::clSetKernelArg(k->getRaw(), arg_idx, size, data);
    free(data);
  }

  // TODO: check for image_t types
  // TODO: support queue_t and clk_event_t, and others?

  // Otherwise it should be a native type
  else if (strcmp(name, "sampler_t") == 0) {
    NOCL_UNWRAP(sw , NoCLSampler, args[2]);
    err = ::clSetKernelArg(k->getRaw(), arg_idx, sizeof(cl_sampler), &sw->getRaw());
  } else {
    return NanThrowError("FIXME: Unsupported OpenCL argument type");
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
#ifdef CL_VERSION_1_2
    case CL_KERNEL_ATTRIBUTES:
#endif
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

#ifdef CL_VERSION_1_2
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
#endif

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
#ifdef CL_VERSION_1_2
    case CL_KERNEL_GLOBAL_WORK_SIZE:
#endif
    case CL_KERNEL_COMPILE_WORK_GROUP_SIZE: {
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
#ifdef CL_VERSION_1_2
  NODE_SET_METHOD(exports, "getKernelArgInfo", GetKernelArgInfo);
#endif
  NODE_SET_METHOD(exports, "getKernelWorkGroupInfo", GetKernelWorkGroupInfo);
}
} //namespace Kernel

} // namespace opencl
