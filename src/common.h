#ifndef OPENCL_COMMON_H_
#define OPENCL_COMMON_H_

// Node includes
#include <node.h>
#include "nan.h"
#include <string>
#include <memory>
#include <vector>
#ifdef LOGGING
#include <iostream>
#endif

using namespace std;
using namespace v8;

// OpenCL includes
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

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
#define JS_STR(...) NanNew<v8::String>(__VA_ARGS__)
#define JS_INT(val) NanNew<v8::Integer>(static_cast<unsigned int>(val))
#define JS_NUM(val) NanNew<v8::Number>(val)
#define JS_BOOL(val) NanNew<v8::Boolean>(val)
#define JS_RETHROW(tc) NanNew<v8::Local<v8::Value> >(tc.Exception());

#define CHECK_ERR(ret)  { cl_int _err=(ret); \
  if ((_err) != CL_SUCCESS) { \
    return NanThrowError(getExceptionMessage(_err).c_str(), _err); \
  } \
}

#define THROW_ERR(code) { cl_int _err=(code); \
  return NanThrowError(getExceptionMessage(_err).c_str(), _err); \
}

#define REQ_ARGS(N)                                                     \
  if (args.Length() < (N)) {                                            \
    NanThrowTypeError("Expected " #N " arguments");                     \
  }

#define REQ_STR_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsString())                     \
    NanThrowTypeError("Argument " #I " must be a string");              \
  String::Utf8Value VAR(args[I]->ToString());

#define REQ_ARRAY_ARG(I, VAR) \
  if (!args[I]->IsArray()) { \
    NanThrowTypeError("Argument " #I " must be an array");              \
      } \
    Local<Array> VAR = Local<Array>::Cast(args[I])

#define REQ_EXT_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsExternal())                   \
    NanThrowTypeError("Argument " #I " invalid");                       \
  Local<External> VAR = Local<External>::Cast(args[I]);

#define REQ_FUN_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsFunction())                   \
    NanThrowTypeError("Argument " #I " must be a function");            \
  Local<Function> VAR = Local<Function>::Cast(args[I]);

} // namespace

namespace opencl {

#define ARG_EXISTS(nth) \
  args.Length() >= nth + 1 && !args[nth]->IsNull() && !args[nth]->IsUndefined()


void getPtrAndLen(const Local<Value> value, void* &ptr, int &len);

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

const std::string getExceptionMessage(const cl_int code);

} // namespace opencl

#endif // OPENCL_COMMON_H_
