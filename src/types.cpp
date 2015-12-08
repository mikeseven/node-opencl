#include "types.h"
#include "common.h"

namespace opencl {

using namespace v8;

const char *cl_type_names[] = {
  "CLPlatformId",
  "CLDeviceId",
  "CLContext",
  "CLProgram",
  "CLKernel",
  "CLMem",
  "CLSampler",
  "CLCommandQueue",
  "CLEvent",
  "CLProgramBinary",
  "CLMappedPtr",
};

static Nan::Persistent<FunctionTemplate> prototypes[11];
static Nan::Persistent<Function> constructors[11];

Nan::Persistent<v8::FunctionTemplate>& prototype(int id) {
  return prototypes[id];
}

Nan::Persistent<v8::Function>& constructor(int id) {
  return constructors[id];
}

namespace Types {
NAN_MODULE_INIT(init)
{
  NoCLPlatformId::Init(target);
  NoCLDeviceId::Init(target);
  NoCLContext::Init(target);
  NoCLProgram::Init(target);
  NoCLKernel::Init(target);
  NoCLMem::Init(target);
  NoCLSampler::Init(target);
  NoCLCommandQueue::Init(target);
  NoCLEvent::Init(target);
  NoCLProgramBinary::Init(target);
  NoCLMappedPtr::Init(target);
}
}

}
