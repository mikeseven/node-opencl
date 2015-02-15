#ifndef NODE_OPENCL_TYPES_H_
#define NODE_OPENCL_TYPES_H_


#include <node.h>
#include "nan.h"
#include <string>
#include <memory>
#include <vector>
#include <v8.h>

#if defined (__APPLE__) || defined(MACOSX)
#ifdef __ECLIPSE__
    #include <gltypes.h>
    #include <gl3.h>
    #include <cl_platform.h>
    #include <cl.h>
    #include <cl_gl.h>
    #include <cl_gl_ext.h>
    #include <cl_ext.h>
  #else
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <OpenGL/OpenGL.h>
#include <OpenCL/opencl.h>
#define CL_GL_CONTEXT_KHR 0x2008
#define CL_EGL_DISPLAY_KHR 0x2009
#define CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR CL_INVALID_GL_CONTEXT_APPLE
#endif
#define HAS_clGetContextInfo
#elif defined(_WIN32)
    #include <GL/gl.h>
    #include <CL/opencl.h>
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
    char *strcasestr(const char *s, char *find);
#else
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <CL/opencl.h>
#endif

using namespace std;
using namespace v8;

template <typename T>
T * NoCLUnwrap(Local<Value> val) {
  if (!val->IsObject()) {
    return NULL;
  }

  Local<Object> obj = val->ToObject();

  if (NanGetInternalFieldPointer(obj, 0) == NULL) {
    return NULL;
  }

  if (NanGetInternalFieldPointer(obj, 1) == NULL) {
    return NULL;
  }

  uint identifier = * (uint *) NanGetInternalFieldPointer(obj, 1);

  if (identifier != T::GetId()) {
    return NULL;
  }

  T * output = (T *) NanGetInternalFieldPointer(obj, 0);
  return output;
}

Local<ObjectTemplate> & GetNodeOpenCLObjectGenericTemplate();

#define NOCL_UNWRAP(V, T, VAL) \
  T * V; \
  V = NoCLUnwrap<T>(VAL);\
  if (V == NULL) return NanThrowTypeError("Bad type of NoCL object")

template <typename T, uint elid>
class NoCLObject {
private:
  T raw;
public:

  NoCLObject(T raw) : raw(raw) {
  }

  T getRaw() {
    return this->raw;
  }

  static uint GetId() {
    return elid;
  }

  template <typename A>
  static bool fromJSArray(std::vector<A> outArr, Local<Array> &arr) {
    for (uint i = 0; i < arr->Length(); ++i) {
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
    for (uint i = 0; i < outArr.size(); ++i) {
      out.push_back(outArr[i].getRaw());
    }

    return out;
  }

};

#define NOCL_TO_ARRAY(TO, FROM, TYPE) \
  if (!TYPE::fromJSArray<TYPE>(TO, FROM)) { \
    return NanThrowTypeError("Bad type of NoCL object in array"); \
  }

#define TO_CL_ARRAY(FROM, TYPE) \
  FROM.size() ? &TYPE::toCLArray<TYPE>(FROM).front() : nullptr

class NoCLPlatformId : public NoCLObject<cl_platform_id, 0> {

public:
  NoCLPlatformId(cl_platform_id raw) : NoCLObject(raw) {
  }
};

class NoCLDeviceId : public NoCLObject<cl_device_id, 1> {

public:
  NoCLDeviceId(cl_device_id raw) : NoCLObject(raw) {
  }
};

class NoCLContext : public NoCLObject<cl_context, 2> {

public:
  NoCLContext(cl_context raw) : NoCLObject(raw) {
  }
};;

class NoCLProgram : public NoCLObject<cl_program, 3> {

public:
  NoCLProgram(cl_program raw) : NoCLObject(raw) {
  }
};

class NoCLKernel : public NoCLObject<cl_kernel, 4> {

public:
  NoCLKernel(cl_kernel raw) : NoCLObject(raw) {
  }
};

class NoCLMem : public NoCLObject<cl_mem, 5> {

public:
  NoCLMem(cl_mem raw) : NoCLObject(raw) {
  }
};

class NoCLSampler : public NoCLObject<cl_sampler, 6> {

public:
  NoCLSampler(cl_sampler raw) : NoCLObject(raw) {
  }
};

class NoCLCommandQueue : public NoCLObject<cl_command_queue, 7> {

public:
  NoCLCommandQueue(cl_command_queue raw) : NoCLObject(raw) {
  }
};

class NoCLEvent : public NoCLObject<cl_event, 8> {

public:
  NoCLEvent(cl_event raw) : NoCLObject(raw) {
  }
};

class NoCLMappedPtr : public NoCLObject<void *, 9> {

public:
  NoCLMappedPtr(void * raw) : NoCLObject(raw) {
  }
};

// FIXME static does not seem to work great with V8 (random segfaults)
// But we should not create a template each time we create an object
template <typename T>
Local<Object> NoCLWrapCLObject(T * elm) {
  Local<ObjectTemplate> tpl = NanNew<ObjectTemplate>();
  tpl->SetInternalFieldCount(2);
  Local<Object> obj = tpl->NewInstance();

  NanSetInternalFieldPointer(obj, 0, elm);
  NanSetInternalFieldPointer(obj, 1, new uint(T::GetId()));
  return obj;
}

#define NOCL_WRAP(T, V) \
  NoCLWrapCLObject<T>(new T(V))

#endif
