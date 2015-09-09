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
   NoCLSVMFreeCallback(NanCallback* callback,const v8::Local<v8::Object> &userData):NanAsyncLaunch(callback){
       NanScope();
       v8::Local<v8::Object> obj = NanNew<v8::Object>();
       NanAssignPersistent(persistentHandle, obj);
       v8::Local<v8::Object>  handle = NanNew(persistentHandle);
       handle->Set(kIndex, userData);
   }

   void Execute() {
     NanScope();
     v8::Local<v8::Object> handle = NanNew(persistentHandle);
     v8::Local<v8::Object> userData= (handle->Get(kIndex)).As<v8::Object>();
     Handle<Value> argv[] = {
         NanNew(userData)
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
  NanScope();
  REQ_ARGS(4);

  // Arg 1
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 2
  cl_svm_mem_flags flags = args[1]->Uint32Value();

  // Arg 2
  cl_uint size = args[2]->Uint32Value();

  // Arg 3
  cl_uint alignment = args[3]->Uint32Value();

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

  Local<Object> buf = NanNewBufferHandle(ptr,size,freeSVMPtr,ctx);

  NanReturnValue(buf);

}


NAN_METHOD(SVMFree) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  void *ptr=nullptr;
  int len=0;
  getPtrAndLen(args[1], ptr, len);
  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_ARG_VALUE);

  svmStatusMap[ptr].deleted = true;
  clSVMFree(context->getRaw(),ptr);

  Local<Object> obj = args[1].As<Object>();
  obj->SetIndexedPropertiesToExternalArrayData(NULL, obj->GetIndexedPropertiesExternalArrayDataType(), 0);

  NanReturnValue(JS_INT(CL_SUCCESS));
}

NAN_METHOD(enqueueSVMFree) {
  NanScope();
  REQ_ARGS(2);
  cl_int err;
  NOCL_UNWRAP(cq, NoCLCommandQueue, args[0]);
  Local<Array> arr= Local<Array>::Cast(args[1]);
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
      it.second->SetIndexedPropertiesToExternalArrayData(it.first, it.second->GetIndexedPropertiesExternalArrayDataType(), 0);
  }


  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(4)) {
    Local<Array> js_events = Local<Array>::Cast(args[4]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(5) && args[5]->BooleanValue())
      eventPtr = &event;

  if (ARG_EXISTS(2)) {
   Local<Function> callbackHandle = args[2].As<Function>();
   NanCallback *callback = new NanCallback(callbackHandle);
   Local<Object> userData = args[3].As<Object>();
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
      NanReturnValue(NOCL_WRAP(NoCLEvent, event));
    } else {
      NanReturnValue(JS_INT(CL_SUCCESS));
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
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
}



NAN_METHOD(enqueueSVMMemcpy) {
  NanScope();
  REQ_ARGS(5);
  cl_int err;

  NOCL_UNWRAP(cq, NoCLCommandQueue, args[0]);
  cl_bool blocking_copy = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;

  void* dst;
  int len=0;
  getPtrAndLen(args[2], dst, len);

  void* src;
  int len2;
  getPtrAndLen(args[3], src, len2);

  size_t size = args[4]->Uint32Value();

  if(size>static_cast<size_t>(len) || size>static_cast<size_t>(len2))
    THROW_ERR(CL_INVALID_VALUE);

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(5)) {
    Local<Array> js_events = Local<Array>::Cast(args[5]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(6) && args[6]->BooleanValue())
      eventPtr = &event;

  err = clEnqueueSVMMemcpy(cq->getRaw(),blocking_copy,
                           dst,src,size,
                           (cl_uint) cl_events.size(),
                           NOCL_TO_CL_ARRAY(
                           cl_events, NoCLEvent),
                           eventPtr);

  CHECK_ERR(err);
  if (eventPtr != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
}

NAN_METHOD(enqueueSVMMemFill) {
  NanScope();
  REQ_ARGS(4);
  cl_int err;

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, args[0]);

  void* ptr;
  int length=0;
  getPtrAndLen(args[1], ptr, length);

  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_VALUE);

  void* pattern;
  int len;
  getPtrAndLen(args[2], pattern, len);
  size_t size = args[3]->Uint32Value();

  if(size>static_cast<size_t>(len) ||
     size >static_cast<size_t>(length) ||
     ((size%len)!=0))
    THROW_ERR(CL_INVALID_VALUE);

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(4)) {
    Local<Array> js_events = Local<Array>::Cast(args[4]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(5) && args[5]->BooleanValue())
      eventPtr = &event;

  err =  clEnqueueSVMMemFill(cq->getRaw(),ptr,pattern,static_cast<size_t>(len),size,
                             (cl_uint) cl_events.size(),
                             NOCL_TO_CL_ARRAY(
                             cl_events, NoCLEvent),
                             eventPtr);
  CHECK_ERR(err);
  if (eventPtr != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
}

NAN_METHOD(enqueueSVMMap) {
  NanScope();
  REQ_ARGS(5);
  cl_int err;

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, args[0]);

  cl_bool blocking_map = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
  cl_map_flags map_flags = args[2]->Uint32Value();

  void* ptr;
  int len=0;
  getPtrAndLen(args[3], ptr, len);

  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_VALUE);

  size_t size = args[4]->Uint32Value();

  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(5)) {
    Local<Array> js_events = Local<Array>::Cast(args[5]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(6) && args[6]->BooleanValue())
      eventPtr = &event;

  err = clEnqueueSVMMap(cq->getRaw(),blocking_map,map_flags,
                        ptr,size, (cl_uint)cl_events.size(),
                        NOCL_TO_CL_ARRAY(
                        cl_events, NoCLEvent),
                        eventPtr);

  CHECK_ERR(err);
  if (eventPtr != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
}

NAN_METHOD(enqueueSVMUnmap) {
  NanScope();
  REQ_ARGS(2);
  cl_int err;

  // Arg 0
  NOCL_UNWRAP(cq, NoCLCommandQueue, args[0]);
  void* ptr;
  int len=0;
  getPtrAndLen(args[1], ptr, len);

  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_VALUE);


  std::vector<NoCLEvent> cl_events;
  if(ARG_EXISTS(2)) {
    Local<Array> js_events = Local<Array>::Cast(args[2]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(3) && args[3]->BooleanValue())
      eventPtr = &event;

  err = clEnqueueSVMUnmap(cq->getRaw(),ptr, (cl_uint)cl_events.size(),
                       NOCL_TO_CL_ARRAY(
                       cl_events, NoCLEvent),
                       eventPtr);

  CHECK_ERR(err)
  if (eventPtr != nullptr) {
    NanReturnValue(NOCL_WRAP(NoCLEvent, event));
  } else {
    NanReturnValue(JS_INT(CL_SUCCESS));
  }
}

NAN_METHOD(setKernelArgSVMPointer) {
  NanScope();
  REQ_ARGS(3);
  cl_int err;

  // Arg 0
  NOCL_UNWRAP(k, NoCLKernel, args[0]);

  // Arg 1
  unsigned int idx = args[1]->Uint32Value();
  void* ptr;
  int len=0;
  getPtrAndLen(args[2], ptr, len);

  if(ptr == NULL || svmStatusMap.count(ptr)==0
          ||svmStatusMap[ptr].deleted )
    THROW_ERR(CL_INVALID_VALUE);

  err = clSetKernelArgSVMPointer(k->getRaw(),idx,ptr);

  CHECK_ERR(err)
  NanReturnValue(JS_INT(CL_SUCCESS));
}


#endif

namespace SVM {
void init(Handle<Object> exports)
{
#ifdef CL_VERSION_2_0
  NODE_SET_METHOD(exports, "SVMAlloc", SVMAlloc);
  NODE_SET_METHOD(exports, "SVMFree", SVMFree);
  NODE_SET_METHOD(exports, "enqueueSVMFree", enqueueSVMFree);
  NODE_SET_METHOD(exports, "enqueueSVMMap", enqueueSVMMap);
  NODE_SET_METHOD(exports, "enqueueSVMMemcpy", enqueueSVMMemcpy);
  NODE_SET_METHOD(exports, "enqueueSVMMemFill", enqueueSVMMemFill);
  NODE_SET_METHOD(exports, "enqueueSVMUnmap", enqueueSVMUnmap);
  NODE_SET_METHOD(exports, "setKernelArgSVMPointer", setKernelArgSVMPointer);
#endif
}
} // namespace Pipe

} // namespace opencl
