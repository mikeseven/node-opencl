#ifndef NODE_OPENCL_TYPES_H_
#define NODE_OPENCL_TYPES_H_

#include <node.h>
#include "nan.h"
#include <string>
#include <vector>
#include <v8.h>
#include <iostream>
#include <sstream>

#if defined (__APPLE__) || defined(MACOSX)
#ifdef __ECLIPSE__
    #include <cl_platform.h>
    #include <cl.h>
    #include <cl_ext.h>
  #else
#include <OpenCL/opencl.h>
#endif
#define HAS_clGetContextInfo
#elif defined(_WIN32)
    #include <CL/opencl.h>
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
    char *strcasestr(const char *s, char *find);
#else
    #include <CL/opencl.h>
#endif

using namespace std;
using namespace v8;

namespace opencl {

extern const char *cl_type_names[];

Nan::Persistent<v8::FunctionTemplate>& prototype(int id);
Nan::Persistent<v8::Function>& constructor(int id);

template <typename T, int id, int err, int cl_release(T), int cl_acquire(T)>
class NoCLWrapper : public Nan::ObjectWrap {
public:
  NoCLWrapper() : raw(nullptr) {
  }

  virtual ~NoCLWrapper() {
  };

  static NAN_MODULE_INIT(Init) {
    Nan::HandleScope scope;
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetPrototypeMethod(tpl, "toString", toString);
    prototype(id).Reset(tpl);
    constructor(id).Reset(tpl->GetFunction());
  }

  static Local<Object> NewInstance(T raw) {
    Local<Function> ctor = Nan::New(constructor(id));
    Local<Object> obj = Nan::NewInstance(ctor, 0, nullptr).ToLocalChecked();
    Unwrap(obj)->raw = raw;
    return obj;
  }

  static NoCLWrapper<T, id, err, cl_release, cl_acquire> *Unwrap(Local<Value> value) {
    if (!value->IsObject())
      return nullptr;
    Local<Object> obj = value->ToObject();
    if (!Nan::New(prototype(id))->HasInstance(obj))
      return nullptr;
    return ObjectWrap::Unwrap<NoCLWrapper<T, id, err, cl_release, cl_acquire> >(obj);
  }

  template <typename A>
  static bool fromJSArray(std::vector<A> & outArr, Local<Array> &arr) {
    for (unsigned int i = 0; i < arr->Length(); ++i) {
      A v = Unwrap(arr->Get(i));
      if (v == NULL) {
	return false;
      }
      outArr.push_back(v);
    }
    return true;
  }

  template <typename U>
  static std::vector<T> toCLArray(std::vector<U> outArr) {
    std::vector<T> out;
    for (unsigned int i = 0; i < outArr.size(); ++i) {
      out.push_back(outArr[i]->getRaw());
    }
    return out;
  }

  T getRaw() const {
    return raw;
  }

  int acquire() const {
    return cl_acquire(raw);
  }

  int release() const {
    return cl_release(raw);
  }

  static int getErrorCode() {
    return err;
  }

private:
  static NAN_METHOD(New) {
    NoCLWrapper<T, id, err, cl_release, cl_acquire> *obj = new NoCLWrapper<T, id, err, cl_release, cl_acquire>();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }

  static NAN_METHOD(toString) {
    NoCLWrapper<T, id, err, cl_release, cl_acquire> *obj = Unwrap(info.This());
    std::stringstream ss;
    if (!obj) {
      Nan::TypeError("not a NoCLWrapper object");
      return;
    }
    ss << "[object " << cl_type_names[id] << "@" << obj->raw << "]";
    info.GetReturnValue().Set(Nan::New<String>(ss.str()).ToLocalChecked());
  }

  T raw;
};

template <typename T>
static inline int noop(T _) {
  return 0;
}

#define NOCL_UNWRAP(VAR, TYPE, EXPR) \
  TYPE * VAR = TYPE::Unwrap(EXPR);	\
  if (VAR == NULL) { \
    Nan::ThrowError(JS_STR(opencl::getExceptionMessage(TYPE::getErrorCode()))); \
    return; \
  }

#define NOCL_TO_ARRAY(TO, FROM, TYPE) \
  if (!TYPE::fromJSArray(TO, FROM)) { \
    Nan::ThrowError(JS_STR(opencl::getExceptionMessage(TYPE::getErrorCode()))); \
    return;\
  }

#define NOCL_TO_CL_ARRAY(FROM, TYPE) \
  FROM.size() ? &TYPE::toCLArray(FROM).front() : nullptr

#define NOCL_WRAPPER(name, type, id, err, release, acquire)	\
  typedef NoCLWrapper<type, id, err, release, acquire> name

typedef const unsigned char *cl_program_binary;
typedef const void *cl_mapped_ptr;

NOCL_WRAPPER(NoCLPlatformId, cl_platform_id, 0, CL_INVALID_PLATFORM, noop, noop);
NOCL_WRAPPER(NoCLDeviceId, cl_device_id, 1, CL_INVALID_DEVICE, noop, noop);
NOCL_WRAPPER(NoCLContext, cl_context, 2, CL_INVALID_CONTEXT, clReleaseContext, clRetainContext);
NOCL_WRAPPER(NoCLProgram, cl_program, 3, CL_INVALID_PROGRAM, clReleaseProgram, clRetainProgram);
NOCL_WRAPPER(NoCLKernel, cl_kernel, 4, CL_INVALID_KERNEL, clReleaseKernel, clRetainKernel);
NOCL_WRAPPER(NoCLMem, cl_mem, 5, CL_INVALID_MEM_OBJECT, clReleaseMemObject, clRetainMemObject);
NOCL_WRAPPER(NoCLSampler, cl_sampler, 6, CL_INVALID_SAMPLER, clReleaseSampler, clRetainSampler);
NOCL_WRAPPER(NoCLCommandQueue, cl_command_queue, 7, CL_INVALID_COMMAND_QUEUE, clReleaseCommandQueue, clRetainCommandQueue);
NOCL_WRAPPER(NoCLEvent, cl_event, 8, CL_INVALID_EVENT, clReleaseEvent, clRetainEvent);
NOCL_WRAPPER(NoCLProgramBinary, cl_program_binary, 9, CL_INVALID_VALUE, noop, noop);
NOCL_WRAPPER(NoCLMappedPtr, cl_mapped_ptr, 10, CL_INVALID_VALUE, noop, noop);

namespace Types {
NAN_MODULE_INIT(init);
}

#define NOCL_WRAP(T, V) \
  T::NewInstance(V)

}
#endif
