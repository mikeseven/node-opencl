#ifndef NODE_OPENCL_TYPES_H_
#define NODE_OPENCL_TYPES_H_

#include <node.h>
#include "nan.h"
#include <string>
#include <vector>
#include <v8.h>
#include <map>
#include<iostream>

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

template <typename T>
class NoCLWrapper : public Nan::ObjectWrap {
public:
  NoCLWrapper();
  virtual ~NoCLWrapper();

  static NAN_MODULE_INIT(Init);
  static Local<Object> NewInstance();
  static T *Unwrap(Local<Value> value);

private:
  static Nan::Persistent<v8::Function> & constructor();
  static Nan::Persistent<v8::FunctionTemplate> & prototype();
  static NAN_METHOD(New);

  T raw;
};

template <typename T>
T * NoCLUnwrap(Local<Value> val) {
  if (val->IsNull() || val->IsUndefined()) {
    return NULL;
  }

  if (!val->IsObject() || val->IsArray()) {
    return NULL;
  }

  Local<Object> obj = val->ToObject();

  if (Nan::GetInternalFieldPointer(obj, 0) == NULL) {
    return NULL;
  }

  if (Nan::GetInternalFieldPointer(obj, 1) == NULL) {
    return NULL;
  }

  unsigned int identifier = * (unsigned int *) Nan::GetInternalFieldPointer(obj, 1);

  if (identifier != T::GetId()) {
    return NULL;
  }


  T * output = (T *) Nan::GetInternalFieldPointer(obj, 0);
  return output;
}

#define NOCL_UNWRAP(VAR, TYPE, EXPR) \
  TYPE * VAR = NoCLUnwrap<TYPE>(EXPR);\
  if (VAR == NULL) { \
    Nan::ThrowError(JS_STR(opencl::getExceptionMessage(TYPE::getErrorCode()))); \
    return; \
  }


class NoCLObjectGen {

 public:

   virtual unsigned int GetType() = 0;

};

template <typename T,const unsigned int elid, int err>
class NoCLObject : public NoCLObjectGen {
public:

  NoCLObject(T raw) {
    this->raw = raw;
  }

  bool operator ==(const NoCLObject<T,elid,err> &b) const {
    return (this->getRaw() == b.getRaw());
  }

  const T& getRaw() const {
    return this->raw;
  }

  // implicit cast
  operator T() const{
      return (T)raw;
  }

  static unsigned int GetId() {
    return elid;
  }

  template <typename A>
  static bool fromJSArray(std::vector<A> & outArr, Local<Array> &arr) {
    for (unsigned int i = 0; i < arr->Length(); ++i) {
      A * v = NoCLUnwrap<A>(arr->Get(i));
      if (v == NULL) {
        return false;
      }
      outArr.push_back(*v);
    }

    return true;
  }

  template <typename U>
  static std::vector<T> toCLArray(std::vector<U> outArr) {
    std::vector<T> out;
    for (unsigned int i = 0; i < outArr.size(); ++i) {
      out.push_back(outArr[i].getRaw());
    }

    return out;
  }

  static cl_uint getErrorCode(){
    return err;
  }

  virtual unsigned int GetType() {
      return elid;
  }

protected:
  T raw;

};

template <typename T, unsigned int elid, int err,int cl_release(T),int cl_acquire(T)>
class NoCLRefCountObject : public NoCLObject<T,elid,err> {
 public:
   NoCLRefCountObject(T raw):NoCLObject<T,elid,err>(raw){
     this->init();
   }

   NoCLRefCountObject(const NoCLRefCountObject &other):NoCLObject<T,elid,err>(other.getRaw()) {
     this->init();
   }

   NoCLRefCountObject& operator = (const NoCLRefCountObject& other){
     this->releaseAndInit(other.getRaw());
     return *this;
   }

   NoCLRefCountObject& operator = (const T& raw){
     this->releaseAndInit(raw);
     return *this;
   }

   virtual ~NoCLRefCountObject() {
     this->release();
   }

   int acquire(bool init=false) {
     return cl_acquire(this->raw);
   }

   int release() {
     return cl_release(this->raw);
   }

 private:
   void init() {
     this->acquire();
   }

   void releaseAndInit(T raw) {
     this->release();
     this->raw = raw;
     this->init();
   }
};

#define NOCL_TO_ARRAY(TO, FROM, TYPE) \
  if (!TYPE::fromJSArray<TYPE>(TO, FROM)) { \
    Nan::ThrowError(JS_STR(opencl::getExceptionMessage(TYPE::getErrorCode()))); \
    return;\
  }

#define NOCL_TO_CL_ARRAY(FROM, TYPE) \
  FROM.size() ? &TYPE::toCLArray<TYPE>(FROM).front() : nullptr

class NoCLPlatformId : public NoCLObject<cl_platform_id, 0, CL_INVALID_PLATFORM> {

public:
  NoCLPlatformId(cl_platform_id raw) : NoCLObject(raw) {
  }

  void release() { }
};

class NoCLDeviceId : public NoCLObject<cl_device_id, 1, CL_INVALID_DEVICE> {

public:
  NoCLDeviceId(cl_device_id raw) : NoCLObject(raw) {
  }

  void release() { }
};

class NoCLContext : public NoCLRefCountObject<cl_context, 2, CL_INVALID_CONTEXT,clReleaseContext,clRetainContext> {

public:
  NoCLContext(cl_context raw) : NoCLRefCountObject(raw) {
  }
};

class NoCLProgram : public NoCLRefCountObject<cl_program, 3, CL_INVALID_PROGRAM,clReleaseProgram,clRetainProgram> {

public:
  NoCLProgram(cl_program raw) : NoCLRefCountObject(raw) {
  }
};

class NoCLKernel : public NoCLRefCountObject<cl_kernel, 4, CL_INVALID_KERNEL,clReleaseKernel,clRetainKernel> {

public:
  NoCLKernel(cl_kernel raw) : NoCLRefCountObject(raw) {
  }
};

class NoCLMem : public NoCLRefCountObject<cl_mem, 5, CL_INVALID_MEM_OBJECT,clReleaseMemObject,clRetainMemObject> {

public:
  NoCLMem(cl_mem raw) : NoCLRefCountObject(raw) {
  }
};

class NoCLSampler : public NoCLRefCountObject<cl_sampler, 6, CL_INVALID_SAMPLER,clReleaseSampler,clRetainSampler> {

public:
  NoCLSampler(cl_sampler raw) : NoCLRefCountObject(raw) {
  }
};

class NoCLCommandQueue : public NoCLRefCountObject<cl_command_queue, 7, CL_INVALID_COMMAND_QUEUE,clReleaseCommandQueue,clRetainCommandQueue> {

public:
  NoCLCommandQueue(cl_command_queue raw) : NoCLRefCountObject(raw) {
  }
};

class NoCLEvent : public NoCLRefCountObject<cl_event, 8, CL_INVALID_EVENT,clReleaseEvent,clRetainEvent> {

public:
  NoCLEvent(cl_event raw) : NoCLRefCountObject(raw) {
  }
};

class NoCLProgramBinary : public NoCLObject<const unsigned char *, 9, CL_INVALID_VALUE> {

public:
  NoCLProgramBinary(const unsigned char * raw) : NoCLObject(raw) {
  }

  void release() { }
};

/*
class NoCLMappedPtr : public NoCLObject<void *, 10, CL_INVALID_VALUE> {

public:
  NoCLMappedPtr(void * raw) : NoCLObject(raw) {
  }
};*/

NAN_METHOD(Equals);

// FIXME static does not seem to work great with V8 (random segfaults)
// But we should not create a template each time we create an object
template <typename T>
Local<Object> NoCLWrapCLObject(T * elm, bool release) {
  Local<ObjectTemplate> tpl = Nan::New<ObjectTemplate>();

  tpl->Set(Nan::New<v8::String>("equals").ToLocalChecked(),
    Nan::New<FunctionTemplate>(Equals, Nan::New<v8::External>(elm)));

  tpl->SetInternalFieldCount(2);
  Local<Object> obj = tpl->NewInstance();

  Nan::SetInternalFieldPointer(obj, 0, elm);
  Nan::SetInternalFieldPointer(obj, 1, new unsigned int(T::GetId()));

  if (release)
    elm->release();

  return obj;
}

namespace Types {
NAN_MODULE_INIT(init);
}

#define NOCL_WRAP(T, V) \
  NoCLWrapCLObject<T>(new T(V), false)

#define NOCL_WRAP_AND_RELEASE(T, V) \
  NoCLWrapCLObject<T>(new T(V), true)

}
#endif
