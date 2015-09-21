#include "pipe.h"
#include "types.h"
#include "common.h"
#include <node_buffer.h>
#include "map"
#include "nanextension.h"

using namespace node;

namespace opencl {

#ifdef CL_VERSION_2_0

class NoCLSVMFreeCallback:public NanAsyncLaunch {
 public:
   NoCLSVMFreeCallback(Nan::Callback* callback,const v8::Local<v8::Object> &userData):NanAsyncLaunch(callback){
       Nan::HandleScope scope;
       v8::Local<v8::Object> obj = Nan::New<v8::Object>();
       persistentHandle.Reset(obj);
       v8::Local<v8::Object>  handle = Nan::New(persistentHandle);
       handle->Set(kIndex, userData);
   }

   void Execute() {
     Nan::HandleScope scope;
     v8::Local<v8::Object> handle = Nan::New(persistentHandle);
     v8::Local<v8::Object> userData= (handle->Get(kIndex)).As<v8::Object>();
     Handle<Value> argv[] = {
         Nan::New(userData)
     };
     callback->Call(1,argv);
   }
};

void CL_CALLBACK notifySVMFree (cl_command_queue queue, cl_uint num_svm_pointers, void *svm_pointers[],void* user_data) {
    NoCLSVMFreeCallback* asyncCB = static_cast<NoCLSVMFreeCallback*>(user_data);
    if(asyncCB!=nullptr) {
      asyncCB->FireAndForget();
    }
}


struct SVMStatus {
    bool deleted;
    bool mapped;
    uint16_t count;
    SVMStatus():deleted(false),
        mapped(false),count(1u) {
    }
};

static std::map<void*,SVMStatus> svmStatusMap;

void freeSVMPtr(char* ptr,void* hint) {
    NoCLContext* ctx = static_cast<NoCLContext*>(hint);
    SVMStatus& st = svmStatusMap[(void*)ptr];
    if(!st.deleted) {
       clSVMFree(ctx->getRaw(),(void*)ptr);
       st.deleted = true;
    }
    st.count--;
    delete ctx;
}

NAN_METHOD(SVMAlloc) {
  Nan::HandleScope scope;
  REQ_ARGS(4);

  // Arg 1
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 2
  cl_svm_mem_flags flags = info[1]->Uint32Value();

  // Arg 2
  cl_uint size = info[2]->Uint32Value();

  // Arg 3
  cl_uint alignment = info[3]->Uint32Value();

  void* mPtr = ::clSVMAlloc (
    context->getRaw(),
    flags,
    size,
    alignment);

  if(mPtr == NULL)
    THROW_ERR(CL_INVALID_ARG_VALUE);

  char* ptr = (char*)mPtr;

  //New wrapper to avoid GC
  NoCLContext* ctx = new NoCLContext(context->getRaw());

  if(svmStatusMap.count(mPtr)){
    svmStatusMap[mPtr].deleted = false;
    svmStatusMap[mPtr].count++;
  }
  else {
    svmStatusMap[mPtr] = SVMStatus();
  }

  Local<Object> buf = Nan::NewBuffer(ptr,size,freeSVMPtr,ctx).ToLocalChecked();

  info.GetReturnValue().Set(buf);
}


NAN_METHOD(SVMFree) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  void *ptr=nullptr;
  int len=0;
  getPtrAndLen(info[1], ptr, len);
  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_ARG_VALUE);

  svmStatusMap[ptr].deleted = true;
  clSVMFree(context->getRaw(),ptr);

  // TODO sets arg[1] to buffer data
  // Local<Object> obj = info[1].As<Object>();
  // obj->SetIndexedPropertiesToExternalArrayData(NULL, obj->GetIndexedPropertiesExternalArrayDataType(), 0);

  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

NAN_METHOD(enqueueSVMFree) {
  Nan::HandleScope scope;
  REQ_ARGS(2);
  cl_int err;
  NOCL_UNWRAP(cq, NoCLCommandQueue, info[0]);
  Local<Array> arr= Local<Array>::Cast(info[1]);
  cl_uint length = arr->Length();
  map<void*,Local<Object> > svmPtr;
  int len;
  void* ptr;
  if(length == 0u) {
    THROW_ERR(CL_INVALID_VALUE)
  }

  for(cl_uint i=0;i<length;++i){
    getPtrAndLen(arr->Get(i),ptr,len);
    if(ptr == NULL || svmStatusMap.count(ptr)==0
            ||svmStatusMap[ptr].deleted )
      THROW_ERR(CL_INVALID_ARG_VALUE);
    if(svmPtr.count(ptr)>0) {
      THROW_ERR(CL_INVALID_ARG_VALUE)
    }
    svmPtr[ptr]=arr->Get(i).As<Object>();
  }

  vector<void*> vec ;
  for(auto const& it:svmPtr) {
      svmStatusMap[it.first].deleted=true;
      vec.push_back(it.first);

      // TODO
      // it.second->SetIndexedPropertiesToExternalArrayData(it.first, it.second->GetIndexedPropertiesExternalArrayDataType(), 0);
  }


  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(4)) {
    Local<Array> js_events = Local<Array>::Cast(info[4]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(5) && info[5]->BooleanValue())
      eventPtr = &event;

  if (ARG_EXISTS(2)) {
   Local<Function> callbackHandle = info[2].As<Function>();
   Nan::Callback *callback = new Nan::Callback(callbackHandle);
   Local<Object> userData = info[3].As<Object>();
   NoCLSVMFreeCallback* cb = new NoCLSVMFreeCallback(callback,userData);
   err = clEnqueueSVMFree(cq->getRaw(),(cl_uint) vec.size(),vec.data(),
                         notifySVMFree,
                         cb,
                         (cl_uint) cl_events.size(),
                         NOCL_TO_CL_ARRAY(
                         cl_events, NoCLEvent),
                         eventPtr);
    CHECK_ERR(err);
    if (eventPtr != nullptr) {
      info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
      return;
    } else {
      info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
      return;
    }
  }

   err = clEnqueueSVMFree(cq->getRaw(),(cl_uint) vec.size(),vec.data(),
                         NULL,
                         NULL,
                         (cl_uint) cl_events.size(),
                         NOCL_TO_CL_ARRAY(
                         cl_events, NoCLEvent),
                         eventPtr);

  CHECK_ERR(err);
  if (eventPtr != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
  }
}



NAN_METHOD(enqueueSVMMemcpy) {
  Nan::HandleScope scope;
  REQ_ARGS(5);
  cl_int err;

  NOCL_UNWRAP(cq, NoCLCommandQueue, info[0]);
  cl_bool blocking_copy = info[1]->BooleanValue() ? CL_TRUE : CL_FALSE;

  void* dst;
  int len=0;
  getPtrAndLen(info[2], dst, len);

  void* src;
  int len2;
  getPtrAndLen(info[3], src, len2);

  size_t size = info[4]->Uint32Value();

  if(size>static_cast<size_t>(len) || size>static_cast<size_t>(len2))
    THROW_ERR(CL_INVALID_VALUE);

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(5)) {
    Local<Array> js_events = Local<Array>::Cast(info[5]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(6) && info[6]->BooleanValue())
      eventPtr = &event;

  err = clEnqueueSVMMemcpy(cq->getRaw(),blocking_copy,
                           dst,src,size,
                           (cl_uint) cl_events.size(),
                           NOCL_TO_CL_ARRAY(
                           cl_events, NoCLEvent),
                           eventPtr);

  CHECK_ERR(err);
  if (eventPtr != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
  }
}

NAN_METHOD(enqueueSVMMemFill) {
  Nan::HandleScope scope;
  REQ_ARGS(4);
  cl_int err;

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, info[0]);

  void* ptr;
  int length=0;
  getPtrAndLen(info[1], ptr, length);

  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_VALUE);

  void* pattern;
  int len;
  getPtrAndLen(info[2], pattern, len);
  size_t size = info[3]->Uint32Value();

  if(size>static_cast<size_t>(len) ||
     size >static_cast<size_t>(length) ||
     ((size%len)!=0))
    THROW_ERR(CL_INVALID_VALUE);

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(4)) {
    Local<Array> js_events = Local<Array>::Cast(info[4]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(5) && info[5]->BooleanValue())
      eventPtr = &event;

  err =  clEnqueueSVMMemFill(cq->getRaw(),ptr,pattern,static_cast<size_t>(len),size,
                             (cl_uint) cl_events.size(),
                             NOCL_TO_CL_ARRAY(
                             cl_events, NoCLEvent),
                             eventPtr);
  CHECK_ERR(err);
  if (eventPtr != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
  }
}

NAN_METHOD(enqueueSVMMap) {
  Nan::HandleScope scope;
  REQ_ARGS(5);
  cl_int err;

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, info[0]);

  cl_bool blocking_map = info[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
  cl_map_flags map_flags = info[2]->Uint32Value();

  void* ptr;
  int len=0;
  getPtrAndLen(info[3], ptr, len);

  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_VALUE);

  size_t size = info[4]->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(5)) {
    Local<Array> js_events = Local<Array>::Cast(info[5]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(6) && info[6]->BooleanValue())
      eventPtr = &event;

  err = clEnqueueSVMMap(cq->getRaw(),blocking_map,map_flags,
                        ptr,size, (cl_uint)cl_events.size(),
                        NOCL_TO_CL_ARRAY(
                        cl_events, NoCLEvent),
                        eventPtr);

  CHECK_ERR(err);
  if (eventPtr != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
  }
}

NAN_METHOD(enqueueSVMUnmap) {
  Nan::HandleScope scope;
  REQ_ARGS(2);
  cl_int err;

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, info[0]);
  void* ptr;
  int len=0;
  getPtrAndLen(info[1], ptr, len);

  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_VALUE);


  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(2)) {
    Local<Array> js_events = Local<Array>::Cast(info[2]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(3) && info[3]->BooleanValue())
      eventPtr = &event;

  err = clEnqueueSVMUnmap(cq->getRaw(),ptr, (cl_uint)cl_events.size(),
                       NOCL_TO_CL_ARRAY(
                       cl_events, NoCLEvent),
                       eventPtr);

  CHECK_ERR(err)
  if (eventPtr != nullptr) {
    info.GetReturnValue().Set(NOCL_WRAP(NoCLEvent, event));
  } else {
    info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
  }
}

NAN_METHOD(setKernelArgSVMPointer) {
  Nan::HandleScope scope;
  REQ_ARGS(3);
  cl_int err;

  // Arg 0
  NOCL_UNWRAP(k, NoCLKernel, info[0]);

  // Arg 1
  unsigned int idx = info[1]->Uint32Value();
  void* ptr;
  int len=0;
  getPtrAndLen(info[2], ptr, len);

  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_VALUE);

  err = clSetKernelArgSVMPointer(k->getRaw(),idx,ptr);

  CHECK_ERR(err)
  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}


#endif

namespace SVM {
NAN_MODULE_INIT(init)
{
#ifdef CL_VERSION_2_0
  Nan::SetMethod(target, "SVMAlloc", SVMAlloc);
  Nan::SetMethod(target, "SVMFree", SVMFree);
  Nan::SetMethod(target, "enqueueSVMFree", enqueueSVMFree);
  Nan::SetMethod(target, "enqueueSVMMap", enqueueSVMMap);
  Nan::SetMethod(target, "enqueueSVMMemcpy", enqueueSVMMemcpy);
  Nan::SetMethod(target, "enqueueSVMMemFill", enqueueSVMMemFill);
  Nan::SetMethod(target, "enqueueSVMUnmap", enqueueSVMUnmap);
  Nan::SetMethod(target, "setKernelArgSVMPointer", setKernelArgSVMPointer);
#endif
}
} // namespace Pipe

} // namespace opencl
