#include "sampler.h"
#include "types.h"

namespace opencl {

// /* Sampler APIs */
// extern CL_API_ENTRY cl_sampler CL_API_CALL
// clCreateSampler(cl_context          /* context */,
//                 cl_bool             /* normalized_coords */,
//                 cl_addressing_mode  /* addressing_mode */,
//                 cl_filter_mode      /* filter_mode */,
//                 cl_int *            /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(CreateSampler) {
  Nan::HandleScope scope;
  REQ_ARGS(4);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 1
  cl_bool normalized_coords = Nan::To<bool>(info[1]).FromJust() ? CL_TRUE : CL_FALSE;

  // Arg 2
  cl_addressing_mode addressing_mode = Nan::To<uint32_t>(info[2]).FromJust();

  // Arg 3
  cl_filter_mode filter_mode = Nan::To<uint32_t>(info[3]).FromJust();

  cl_int ret=CL_SUCCESS;
  cl_sampler sw = ::clCreateSampler(
              context->getRaw(),
              normalized_coords,
              addressing_mode,
              filter_mode,
              &ret);
  CHECK_ERR(ret);
  info.GetReturnValue().Set(NOCL_WRAP(NoCLSampler, sw));
}

#ifdef CL_VERSION_2_0
// extern CL_API_ENTRY cl_sampler CL_API_CALL
// clCreateSamplerWithProperties(cl_context                     /* context */,
//                               const cl_sampler_properties *  /* normalized_coords */,
//                               cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_2_0;
//
NAN_METHOD(CreateSamplerWithProperties) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  Local<Array> properties = Local<Array>::Cast(info[1]);
  vector<cl_sampler_properties> cl_properties;

  for (uint32_t i=0; i < properties->Length(); i+=2) {
    cl_uint prop_id = Nan::To<int32_t>(Nan::Get(properties, i).ToLocalChecked()).FromJust();
    cl_properties.push_back(prop_id);
    if(prop_id == CL_SAMPLER_NORMALIZED_COORDS) {
      if (!Nan::Get(properties, i+1).ToLocalChecked()->IsBoolean()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_bool norm = Nan::To<bool>(Nan::Get(properties, i+1).ToLocalChecked()).FromJust() ? 1 : 0;
      cl_properties.push_back(norm);
    } else if (prop_id == CL_SAMPLER_ADDRESSING_MODE) {
      if (!Nan::Get(properties, i+1).ToLocalChecked()->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_addressing_mode addr = Nan::To<int32_t>(Nan::Get(properties, i+1).ToLocalChecked()).FromJust();
      cl_properties.push_back(addr);
    } else if (prop_id == CL_SAMPLER_FILTER_MODE) {
      if (!Nan::Get(properties, i+1).ToLocalChecked()->IsNumber()) {
        THROW_ERR(CL_INVALID_VALUE);
      }
      cl_filter_mode fil = Nan::To<int32_t>(Nan::Get(properties, i+1).ToLocalChecked()).FromJust();
      cl_properties.push_back(fil);
    } else {
      THROW_ERR(CL_INVALID_VALUE)
    }
  }
  cl_properties.push_back(0);


  /*cl_sampler_properties  pp [] = {CL_SAMPLER_NORMALIZED_COORDS, true,
   CL_SAMPLER_ADDRESSING_MODE, CL_ADDRESS_NONE,
   CL_SAMPLER_FILTER_MODE, CL_FILTER_LINEAR, 0};*/

  cl_int err = CL_SUCCESS;
  cl_sampler sw = ::clCreateSamplerWithProperties(
              context->getRaw(),
              cl_properties.data(),
              &err);
  CHECK_ERR(err);

  info.GetReturnValue().Set(NOCL_WRAP(NoCLSampler, sw));
}
#endif

// extern CL_API_ENTRY cl_int CL_API_CALL
// clRetainSampler(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(RetainSampler) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(sampler, NoCLSampler, info[0]);

  cl_int err=sampler->acquire();
  CHECK_ERR(err)

  info.GetReturnValue().Set(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clReleaseSampler(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(ReleaseSampler) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  NOCL_UNWRAP(sampler, NoCLSampler, info[0]);
  cl_int err=sampler->release();

  CHECK_ERR(err)
  info.GetReturnValue().Set(JS_INT(err));
}

// extern CL_API_ENTRY cl_int CL_API_CALL
// clGetSamplerInfo(cl_sampler         /* sampler */,
//                  cl_sampler_info    /* param_name */,
//                  size_t             /* param_value_size */,
//                  void *             /* param_value */,
//                  size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;
NAN_METHOD(GetSamplerInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  NOCL_UNWRAP(sampler, NoCLSampler, info[0]);

  cl_sampler_info param_name = Nan::To<uint32_t>(info[1]).FromJust();

  switch(param_name) {
    case CL_SAMPLER_REFERENCE_COUNT:
    {
      cl_uint val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_SAMPLER_CONTEXT:
    {
      cl_context val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_context), &val, NULL))
      info.GetReturnValue().Set(NOCL_WRAP(NoCLContext, val));
      return;
    }
    case CL_SAMPLER_NORMALIZED_COORDS:
    {
      cl_bool val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_bool), &val, NULL))
      info.GetReturnValue().Set(val==CL_TRUE ? Nan::True() : Nan::False());
      return;
    }
    case CL_SAMPLER_ADDRESSING_MODE:
    {
      cl_addressing_mode val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_addressing_mode), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
    case CL_SAMPLER_FILTER_MODE:
    {
      cl_filter_mode val;
      CHECK_ERR(::clGetSamplerInfo(sampler->getRaw(),param_name,sizeof(cl_filter_mode), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
  }
  THROW_ERR(CL_INVALID_VALUE);
}

namespace Sampler {
NAN_MODULE_INIT(init)
{
  Nan::SetMethod(target, "retainSampler", RetainSampler);
  Nan::SetMethod(target, "releaseSampler", ReleaseSampler);
  Nan::SetMethod(target, "getSamplerInfo", GetSamplerInfo);
  Nan::SetMethod(target, "createSampler", CreateSampler);
#ifdef CL_VERSION_2_0
  Nan::SetMethod(target, "createSamplerWithProperties", CreateSamplerWithProperties);
#endif
}
} // namespace Sampler

} // namespace opencl
