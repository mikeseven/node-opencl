#ifndef OPENCL_COMMON_H_
#define OPENCL_COMMON_H_

// Node includes
#include <nan.h>
#include <string>
#include <memory>
#include <vector>
#ifdef LOGGING
#include <iostream>
#endif

using namespace std;
using namespace v8;
using namespace Nan;


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

#ifndef CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR
  #define CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR 0x2006
#endif
#ifndef CL_DEVICES_FOR_GL_CONTEXT_KHR
  #define CL_DEVICES_FOR_GL_CONTEXT_KHR 0x2007
#endif

namespace {
#define JS_STR(...) Nan::New<v8::String>(__VA_ARGS__).ToLocalChecked()
#define JS_INT(val) Nan::New<v8::Integer>(static_cast<unsigned int>(val))
#define JS_NUM(val) Nan::New<v8::Number>(val)
//#define JS_BOOL(val) Nan::New<v8::Boolean>(val)
#define JS_RETHROW(tc) Nan::New<v8::Local<v8::Value> >(tc.Exception());

#define CHECK_ERR(ret)  { cl_int _err=(ret); \
  if ((_err) != CL_SUCCESS) { \
    return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(_err))); \
  } \
}

#define THROW_ERR(code) { cl_int _err=(code); \
  return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(_err))); \
}

#define REQ_ARGS(N)                                                     \
  if (info.Length() < (N)) {                                            \
    Nan::ThrowTypeError("Expected " #N " arguments");                     \
    return;                                               \
  }

#define REQ_STR_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I]->IsString())  {                  \
    Nan::ThrowTypeError("Argument " #I " must be a string");              \
    return;                                               \
  }                                                                     \
  String::Utf8Value VAR(info[I]->ToString());

#define REQ_ARRAY_ARG(I, VAR) \
  if (!info[I]->IsArray()) { \
    Nan::ThrowTypeError("Argument " #I " must be an array");              \
    return;                                               \
  }                                                                     \
  Local<Array> VAR = Local<Array>::Cast(info[I])

} // namespace

namespace opencl {

#define ARG_EXISTS(nth) \
  (info.Length() >= nth + 1 && !info[nth]->IsNull() && !info[nth]->IsUndefined())


void getPtrAndLen(const Local<Value> value, void* &ptr, size_t &len);

//template<typename CL_TYPE>
//void getValuesFromArray(const Local<Array>& arr, std::vector<CL_TYPE>& vals)
//{
//  size_t num=arr->Length();
//  if(!num) {
//    vals.clear();
//    return;
//  }
//
//  vals.reserve(num);
//  for(size_t i=0;i<num;i++)
//    vals.push_back()
//    vals.push_back(Unwrap<CL_TYPE>(arr->Get(i)));
//}

const char* getExceptionMessage(const cl_int code);

} // namespace opencl

#endif // OPENCL_COMMON_H_
