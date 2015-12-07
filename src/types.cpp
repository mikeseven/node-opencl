#include "types.h"
#include "common.h"

namespace opencl {

using namespace v8;

NAN_METHOD(Equals) {
  Nan::HandleScope scope;
  REQ_ARGS(1);

  v8::Handle<v8::External> data = v8::Handle<v8::External>::Cast(info.Data());

  NoCLObjectGen * self = static_cast<NoCLObjectGen *>(data->Value());

  if (!info[0]->IsObject()) {
    info.GetReturnValue().Set(Nan::New<Boolean>(false));
    return;
  }

  Local<Object> otherObj = info[0]->ToObject();
  NoCLObjectGen * other = static_cast<NoCLObjectGen *>(Nan::GetInternalFieldPointer(otherObj, 0));

  if(self->GetType() != other->GetType()) {
    info.GetReturnValue().Set(Nan::New<Boolean>(false));
    return;
  }

  switch (self->GetType()) {

#define NO_CL_COMPARE_OBJ(NAME,NB) \
  case NB: {\
    NAME * aself = static_cast<NAME *>(self);\
    NAME * aother = static_cast<NAME *>(other);\
    info.GetReturnValue().Set(Nan::New<Boolean>((*aother)==(*aself))); }\
    return;

    NO_CL_COMPARE_OBJ(NoCLPlatformId,0)
    NO_CL_COMPARE_OBJ(NoCLDeviceId,1)
    NO_CL_COMPARE_OBJ(NoCLContext,2)
    NO_CL_COMPARE_OBJ(NoCLProgram,3)
    NO_CL_COMPARE_OBJ(NoCLKernel,4)
    NO_CL_COMPARE_OBJ(NoCLMem,5)
    NO_CL_COMPARE_OBJ(NoCLSampler,6)
    NO_CL_COMPARE_OBJ(NoCLCommandQueue,7)
    NO_CL_COMPARE_OBJ(NoCLEvent,8)
    NO_CL_COMPARE_OBJ(NoCLProgramBinary,9)
    //NO_CL_COMPARE_OBJ(NoCLMappedPtr,10)
  default:
    info.GetReturnValue().Set(Nan::New<Boolean>(false));
    return;
  }


}

int myCpt = 0;

namespace Types {
NAN_MODULE_INIT(init)
{
}

}
}
