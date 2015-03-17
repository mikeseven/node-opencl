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
T * NoCLUnwrap(Local<Value> val) {
  if (val->IsNull() || val->IsUndefined()) {
    return NULL;
  }

  if (!val->IsObject() || val->IsArray()) {
    return NULL;
  }

  Local<Object> obj = val->ToObject();

  if (NanGetInternalFieldPointer(obj, 0) == NULL) {
    return NULL;
  }

  if (NanGetInternalFieldPointer(obj, 1) == NULL) {
    return NULL;
  }

  unsigned int identifier = * (unsigned int *) NanGetInternalFieldPointer(obj, 1);

  if (identifier != T::GetId()) {
    return NULL;
  }


  T * output = (T *) NanGetInternalFieldPointer(obj, 0);
  return output;
}

Local<ObjectTemplate> & GetNodeOpenCLObjectGenericTemplate();

#define NOCL_UNWRAP(VAR, TYPE, EXPR) \
  TYPE * VAR = NoCLUnwrap<TYPE>(EXPR);\
  if (VAR == NULL) { NanThrowError(opencl::getExceptionMessage(TYPE::getErrorCode()).c_str(), TYPE::getErrorCode()); NanReturnUndefined(); }


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

  const T getRaw() const {
    return (T) this->raw;
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

extern int myCpt;

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

   bool isDeleted() const{
     if(myMap[this->raw]>0)
         return true;
     return false;
   }

   int acquire(bool init=false) {
#ifdef NOCL_REALEASE_DRIVER_ISSUES
     if(myMap[this->raw]<1)
       return CL_INVALID_ARG_VALUE;
#endif
     cl_int result = cl_acquire(this->raw);
     if(result == CL_SUCCESS)
        myMap[this->raw]++;
     return result;
   }

   int release() {
     int result;

#ifdef NOCL_REALEASE_DRIVER_ISSUES
     if(myMap[this->raw]>1) {
       myMap[this->raw]--;
       result = cl_release(this->raw);
     }
     else {
       result = CL_SUCCESS;
     }
#else
     if(myMap.count(this->raw)>0)
       myMap[this->raw]--;
     result = cl_release(this->raw);
#endif
     return result;
   }

#ifdef NOCL_REALEASE_DRIVER_ISSUES
   static void releaseAll(){
     for (auto const & kv : myMap) {
       for(int cpt = 0;cpt < kv.second;++cpt){
         cl_release(kv.first);
       }
     }
   }

#endif

 protected:
   static std::map<T,int> myMap;

 private:
   void init() {
     if(myMap.count(this->raw)>0) {
       this->acquire();
         //if(result != CL_SUCCESS)
         //  throw result;
       }
       else
         myMap[this->raw] = 1;
   }

   void releaseAndInit(T raw) {
       this->release();
       //if(result!=CL_SUCCESS)
       //    throw result;
       this->raw = raw;
       this->init();
   }


};

#define NOCL_TO_ARRAY(TO, FROM, TYPE) \
  if (!TYPE::fromJSArray<TYPE>(TO, FROM)) { \
    NanThrowError(opencl::getExceptionMessage(TYPE::getErrorCode()).c_str(), TYPE::getErrorCode()); \
    NanReturnUndefined();\
  }

#define NOCL_TO_CL_ARRAY(FROM, TYPE) \
  FROM.size() ? &TYPE::toCLArray<TYPE>(FROM).front() : nullptr

class NoCLPlatformId : public NoCLObject<cl_platform_id, 0, CL_INVALID_PLATFORM> {

public:
  NoCLPlatformId(cl_platform_id raw) : NoCLObject(raw) {
  }
};

class NoCLDeviceId : public NoCLObject<cl_device_id, 1, CL_INVALID_DEVICE> {

public:
  NoCLDeviceId(cl_device_id raw) : NoCLObject(raw) {
  }
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
};

/*
class NoCLMappedPtr : public NoCLObject<void *, 10, CL_INVALID_VALUE> {

public:
  NoCLMappedPtr(void * raw) : NoCLObject(raw) {
  }
};*/

NAN_METHOD(Equals);

NAN_METHOD(releaseAll);

// FIXME static does not seem to work great with V8 (random segfaults)
// But we should not create a template each time we create an object
template <typename T>
Local<Object> NoCLWrapCLObject(T * elm) {
  Local<ObjectTemplate> tpl = NanNew<ObjectTemplate>();

  tpl->Set(NanNew<v8::String>("equals"),
    NanNew<FunctionTemplate>(Equals, NanNew<v8::External>(elm)));

  tpl->SetInternalFieldCount(2);
  Local<Object> obj = tpl->NewInstance();

  NanSetInternalFieldPointer(obj, 0, elm);
  NanSetInternalFieldPointer(obj, 1, new unsigned int(T::GetId()));
  return obj;
}

namespace Types {
  void init(Handle<Object> exports);
}

#define NOCL_WRAP(T, V) \
  NoCLWrapCLObject<T>(new T(V))

}
#endif
