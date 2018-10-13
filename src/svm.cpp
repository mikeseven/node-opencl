#include "pipe.h"
#include "types.h"
#include "common.h"
#include <uv.h>
#include <node_buffer.h>
#include "map"
#include "nanextension.h"

using namespace node;

namespace opencl {

#ifdef CL_VERSION_2_0

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

  Local<v8::ArrayBuffer> obj = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), mPtr, size);

  info.GetReturnValue().Set(obj);
}


NAN_METHOD(SVMFree) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  void *ptr=nullptr;
  size_t len=0;
  getPtrAndLen(info[1], ptr, len);
  if(!ptr || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  clSVMFree(context->getRaw(),ptr);

  // TODO sets arg[1] to buffer data
  // Local<Object> obj = info[1].As<Object>();
  // obj->SetIndexedPropertiesToExternalArrayData(NULL, obj->GetIndexedPropertiesExternalArrayDataType(), 0);

  info.GetReturnValue().Set(JS_INT(CL_SUCCESS));
}

static void WorkAsync(uv_work_t *req);
static void WorkAsyncComplete(uv_work_t *req,int status);

class NoCLSVMWorker : public AsyncWorker
{
public:
  uv_work_t _request;
  uv_sem_t _sem_work;
  
public:
  NoCLSVMWorker(Callback* callback, const v8::Local<v8::Object> &userData,const v8::Local<v8::Object> &noCLCommandQueue) :
  AsyncWorker(callback)
  {
    SaveToPersistent(kIndex,noCLCommandQueue);
    SaveToPersistent(kIndex+1,  userData);
    
    _request.data = this;
    uv_sem_init(&_sem_work,0);
    
    // kick of the worker/waiter thread
    uv_queue_work(uv_default_loop(),&_request,WorkAsync,WorkAsyncComplete);
  }

  ~NoCLSVMWorker() {}

  // Executed inside the worker-thread.
  // not safe to use V8 calls
  void Execute()
  {
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    Nan::EscapableHandleScope  scope;

    Local<Value> argv[] = {
      GetFromPersistent(kIndex),  // CommandQueue
      GetFromPersistent(kIndex+1) // userData
    };
    callback->Call(2,argv);
  }

protected:
  static const uint32_t kIndex = 0;
};

// TODO should we return the svm_pointers to JS callback?
void CL_CALLBACK notifySVMCB ( cl_command_queue queue, cl_uint num_svm_pointers, void *svm_pointers[], void *user_data) {
  NoCLSVMWorker* work = static_cast<NoCLSVMWorker*>(user_data);
  uv_sem_post(&work->_sem_work);
}

// called by libuv worker in separate thread
static void WorkAsync(uv_work_t *req)
{
    NoCLSVMWorker *work = static_cast<NoCLSVMWorker *>(req->data);
    uv_sem_wait(&work->_sem_work);
}

// called by libuv in event loop when async function completes
static void WorkAsyncComplete(uv_work_t *req,int status)
{
    Isolate * isolate = Isolate::GetCurrent();

    // Fix for Node 4.x - thanks to https://github.com/nwjs/blink/commit/ecda32d117aca108c44f38c8eb2cb2d0810dfdeb
    v8::HandleScope handleScope(isolate);

    NoCLSVMWorker *work = static_cast<NoCLSVMWorker *>(req->data);
  
    AsyncQueueWorker(work);
}


NAN_METHOD(enqueueSVMFree) {
  Nan::HandleScope scope;
  REQ_ARGS(2);
  cl_int err;
  NOCL_UNWRAP(cq, NoCLCommandQueue, info[0]);
  Local<Array> arr= Local<Array>::Cast(info[1]);
  cl_uint length = arr->Length();
  if(length == 0u) {
    THROW_ERR(CL_INVALID_VALUE)
  }

  vector<void*> vec ;
  for(cl_uint i=0;i<length;++i){
    size_t len=0;
    void* ptr=nullptr;
    getPtrAndLen(arr->Get(i),ptr,len);
    if(!ptr || !len) {
      return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
    }
    vec.push_back(ptr);
  }

  std::vector<NoCLEvent*> cl_events;
  if(ARG_EXISTS(4)) {
    Local<Array> js_events = Local<Array>::Cast(info[4]);
    NOCL_TO_ARRAY(cl_events, js_events, NoCLEvent);
  }
  cl_event* eventPtr = nullptr;
  cl_event event;

  if(ARG_EXISTS(5) && info[5]->BooleanValue())
      eventPtr = &event;

  if (ARG_EXISTS(2)) {
    Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());
    Local<Object> userData = info[3].As<Object>();

    NoCLSVMWorker* cb = new NoCLSVMWorker(callback,userData,info[0].As<Object>());

    err = clEnqueueSVMFree(cq->getRaw(),(cl_uint) vec.size(),vec.data(),
                            notifySVMCB,
                            cb,
                            (cl_uint) cl_events.size(),
                            NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
                            eventPtr);
  }
  else {
   err = clEnqueueSVMFree(cq->getRaw(),(cl_uint) vec.size(),vec.data(),
                         NULL,
                         NULL,
                         (cl_uint) cl_events.size(),
                         NOCL_TO_CL_ARRAY(cl_events, NoCLEvent),
                         eventPtr);
  }

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

  void* dst=nullptr;
  size_t len=0;
  getPtrAndLen(info[2], dst, len);
    if(!dst || !len) {
      return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
    }

  void* src=nullptr;
  size_t len2=0;
  getPtrAndLen(info[3], src, len2);
  if(!src || !len2) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  size_t size = info[4]->Uint32Value();

  if(size>static_cast<size_t>(len) || size>static_cast<size_t>(len2))
    THROW_ERR(CL_INVALID_VALUE);

  std::vector<NoCLEvent*> cl_events;
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

  void* ptr=nullptr;
  size_t length=0;
  getPtrAndLen(info[1], ptr, length);
  if(!ptr || !length) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  void* pattern=nullptr;
  size_t len=0;
  getPtrAndLen(info[2], pattern, len);
  if(!pattern || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }
  size_t size = info[3]->Uint32Value();

  if(size>static_cast<size_t>(len) ||
     size >static_cast<size_t>(length) ||
     ((size%len)!=0))
    THROW_ERR(CL_INVALID_VALUE);

  std::vector<NoCLEvent*> cl_events;
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

  void* ptr=nullptr;
  size_t len=0;
  getPtrAndLen(info[3], ptr, len);
  if(!ptr || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  size_t size = info[4]->Uint32Value();

  std::vector<NoCLEvent*> cl_events;
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
  void* ptr=nullptr;
  size_t len=0;
  getPtrAndLen(info[1], ptr, len);
  if(!ptr || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

  std::vector<NoCLEvent*> cl_events;
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
  void* ptr=nullptr;
  size_t len=0;
  getPtrAndLen(info[2], ptr, len);
  if(!ptr || !len) {
    return Nan::ThrowTypeError("Unsupported type of buffer. Use node's Buffer or JS' ArrayBuffer");
  }

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
