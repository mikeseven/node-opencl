#ifndef NAN_VERSION
#define NAN_VERSION

#include "nan.h"

//// async Fire and Forget call ////

void NanAsyncAddInQueue(uv_async_t *handle, int status);

/**
  * Allow safe exection of a function from another thread
  *
  * NB:The use of this class is for only one call to avoid the problem in uv_async_call:
  * "not every call to it will yield an execution of the callback." see warning in
  * http://docs.libuv.org/en/latest/async.html#c.uv_async_send
  *
  **/
/* abstract */ class NanAsyncLaunch {
 public:
  NanAsyncLaunch():alreadyFired(false) {
    uv_async_init(uv_default_loop(), &(this->async), NanAsyncLaunch::AddInQueue);
    async.data = this;
  }

  void FireAndForget() {
    if(!alreadyFired) {
      alreadyFired=true;
      uv_async_send(&(this->async));
    }
  }
  virtual void Execute() = 0;

  virtual ~NanAsyncLaunch() {
  }

  virtual void Destroy() {
    uv_close(reinterpret_cast<uv_handle_t*>(&this->async),NanAsyncLaunch::AsyncClose_);
  }
  NAN_INLINE static void AddInQueue(uv_async_t *handle) {
      NanAsyncLaunch* asyncLaunch = static_cast<NanAsyncLaunch*>(handle->data);
      //if(status != UV_ECANCELED)
          asyncLaunch->Execute();
      asyncLaunch->Destroy();
  }

  NAN_INLINE static void AsyncClose_(uv_handle_t* handle) {
      NanAsyncLaunch *launcher =
              static_cast<NanAsyncLaunch*>(handle->data);
      delete launcher;
    }

 private:
  uv_async_t async;
  bool alreadyFired;
};




#endif // NANEXTENSION

