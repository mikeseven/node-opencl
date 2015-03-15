#include "types.h"
#include "common.h"

namespace opencl {

template <typename T, unsigned int elid, int err,int cl_release(T),int cl_acquire(T)> std::map<T,int>NoCLRefCountObject<T,elid,err,cl_release,cl_acquire>::myMap;

NAN_METHOD(Equals) {
  NanScope();
  REQ_ARGS(1);

  v8::Handle<v8::External> data = v8::Handle<v8::External>::Cast(args.Data());

  NoCLObjectGen * self = static_cast<NoCLObjectGen *>(data->Value());

  if (!args[0]->IsObject()) {
    NanReturnValue(NanNew<Boolean>(false));
  }

  Local<Object> otherObj = args[0]->ToObject();
  NoCLObjectGen * other = static_cast<NoCLObjectGen *>(NanGetInternalFieldPointer(otherObj, 0));

  if(self->GetType() != other->GetType()) {
    NanReturnValue(NanNew<Boolean>(false));
  }

  switch (self->GetType()) {

#define NO_CL_COMPARE_OBJ(NAME,NB) \
  case NB: {\
    NAME * aself = static_cast<NAME *>(self);\
    NAME * aother = static_cast<NAME *>(other);\
    NanReturnValue(NanNew<Boolean>((*aother)==(*aself))); }

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
    NO_CL_COMPARE_OBJ(NoCLMappedPtr,10)
  default:
    NanReturnValue(NanNew<Boolean>(false));
  }


}

NAN_METHOD(releaseAll){
  // be careful with the order of the releases: could segfault if the order is not good
  // on some drivers
  NoCLEvent::releaseAll();
  NoCLSampler::releaseAll();
  NoCLMem::releaseAll();
  NoCLKernel::releaseAll();
  NoCLProgram::releaseAll();
  NoCLCommandQueue::releaseAll();
  NoCLContext::releaseAll();
}

int myCpt = 0;

namespace Types {
void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "releaseAll", releaseAll);
}

}
}
