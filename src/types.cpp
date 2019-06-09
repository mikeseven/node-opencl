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
NAN_METHOD(releaseAll){
  // force GC to trigger release of lingering OpenCL objects
  static const int idle_time_in_ms = 5;
  Nan::IdleNotification(idle_time_in_ms);

// TODO: Validate https://youtu.be/iQu9yG1dlrM?t=839
  // be careful with the order of the releases: could segfault if the order is not good
  // on some drivers
  // NoCLEvent::releaseAll();
  // NoCLSampler::releaseAll();
  // NoCLMem::releaseAll();
  // NoCLKernel::releaseAll();
  // NoCLProgram::releaseAll();
  // NoCLCommandQueue::releaseAll();
  // NoCLContext::releaseAll();
}

NAN_MODULE_INIT(init)
{
  Nan::SetMethod(target, "releaseAll", releaseAll);

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
