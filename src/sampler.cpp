#include "sampler.h"
#include "types.h"

namespace opencl {

#ifndef CL_VERSION_2_0
// /* Sampler APIs */
// extern CL_API_ENTRY cl_sampler CL_API_CALL
// clCreateSampler(cl_context          /* context */,
//                 cl_bool             /* normalized_coords */,
//                 cl_addressing_mode  /* addressing_mode */,
//                 cl_filter_mode      /* filter_mode */,
//                 cl_int *            /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateSampler) {
  NanScope();
  REQ_ARGS(4);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 1
  cl_bool normalized_coords = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;

  // Arg 2
  cl_addressing_mode addressing_mode = args[2]->Uint32Value();

  // Arg 3
  cl_filter_mode filter_mode = args[3]->Uint32Value();

  cl_int ret=CL_SUCCESS;
  cl_sampler sw = ::clCreateSampler(
              context->getRaw(),
              normalized_coords,
              addressing_mode,
              filter_mode,
              &ret);
  CHECK_ERR(ret);
  NanReturnValue(NOCL_WRAP(NoCLSampler, sw));
}

#else

// extern CL_API_ENTRY cl_sampler CL_API_CALL
// clCreateSamplerWithProperties(cl_context                     /* context */,
//                               const cl_sampler_properties *  /* normalized_coords */,
//                               cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_2_0;
//                               
NAN_METHOD(CreateSamplerWithProperties) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  Local<Array> properties = Local<Array>::Cast(args[1]);
  vector<cl_sampler_properties> cl_properties;

  for (uint32_t i=0; i < properties->Length(); i+=2) {
    cl_uint prop_id = properties->Get(i)->Int32Value();
    cl_properties.push_back(prop_id);
    if(prop_id == CL_SAMPLER_NORMALIZED_COORDS) {
      if (!properties->Get(i+1)->IsBoolean()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_bool norm = properties->Get(i+1)->BooleanValue() ? 1 : 0;
      cl_properties.push_back(norm);
    } else if (prop_id == CL_SAMPLER_ADDRESSING_MODE) {
      if (!properties->Get(i+1)->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_addressing_mode addr = properties->Get(i+1)->Int32Value();
      cl_properties.push_back(addr);
    } else if (prop_id == CL_SAMPLER_FILTER_MODE) {
      if (!properties->Get(i+1)->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_filter_mode fil = properties->Get(i+1)->Int32Value();
      cl_properties.push_back(fil);
    } else {
      THROW_ERR(CL_INVALID_VALUE)
    }
  }
  cl_properties.push_back(0);


  cl_sampler_properties  pp [] = {CL_SAMPLER_NORMALIZED_COORDS, true,
   CL_SAMPLER_ADDRESSING_MODE, CL_ADDRESS_NONE,
   CL_SAMPLER_FILTER_MODE, CL_FILTER_LINEAR, 0};

  cl_int err = CL_SUCCESS;
  cl_sampler sw = ::clCreateSamplerWithProperties(
              context->getRaw(),
              cl_properties.data(),
              &err);
  CHECK_ERR(err);

  NanReturnValue(NOCL_WRAP(NoCLSampler, sw));


}
#endif

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainSampler(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainSampler) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(sampler, NoCLSampler, args[0]);

  cl_int err=sampler->acquire();
  CHECK_ERR(err)

  NanReturnValue(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseSampler(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseSampler) {
  NanScope();
  REQ_ARGS(1);

  NOCL_UNWRAP(sampler, NoCLSampler, args[0]);
  cl_int err=sampler->release();

  CHECK_ERR(err)
  NanReturnValue(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetSamplerInfo(cl_sampler         /* sampler */,
//                  cl_sampler_info    /* param_name */,
//                  size_t             /* param_value_size */,
//                  void *             /* param_value */,
//                  size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetSamplerInfo) {
  NanScope();
  REQ_ARGS(2);

  NOCL_UNWRAP(sampler, NoCLSampler, args[0]);

  cl_sampler_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_SAMPLER_REFERENCE_COUNT:
    {
      cl_uint val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_SAMPLER_CONTEXT:
    {
      cl_context val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_context), &val, NULL))

      break;
    }
    case CL_SAMPLER_NORMALIZED_COORDS:
    {
      cl_bool val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_bool), &val, NULL))
      NanReturnValue(JS_BOOL(val));
    }
    case CL_SAMPLER_ADDRESSING_MODE:
    {
      cl_addressing_mode val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_addressing_mode), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
    case CL_SAMPLER_FILTER_MODE:
    {
      cl_filter_mode val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_filter_mode), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
  }
  THROW_ERR(CL_INVALID_VALUE);
}

namespace Sampler {
void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "retainSampler", RetainSampler);
  NODE_SET_METHOD(exports, "releaseSampler", ReleaseSampler);
  NODE_SET_METHOD(exports, "getSamplerInfo", GetSamplerInfo);
#ifndef CL_VERSION_2_0
  NODE_SET_METHOD(exports, "createSampler", CreateSampler);
#else
  NODE_SET_METHOD(exports, "createSamplerWithProperties", CreateSamplerWithProperties);
#endif
}
} // namespace Sampler

} // namespace opencl
