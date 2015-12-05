#include "pipe.h"
#include "types.h"
#include "common.h"
#include <node_buffer.h>

using namespace node;

namespace opencl {

#ifdef CL_VERSION_2_0

NAN_METHOD(CreatePipe) {
  Nan::HandleScope scope;
  REQ_ARGS(5);

  // Arg 1
  NOCL_UNWRAP(context, NoCLContext, info[0]);

  // Arg 2
  cl_mem_flags flags = info[1]->Uint32Value();

  // Arg 2
  cl_uint size = info[2]->Uint32Value();

  // Arg 3
  cl_uint qty = info[3]->Uint32Value();

  // Arg 4
  if (!info[4]->IsNull()) {
    THROW_ERR(CL_INVALID_VALUE)
  }

  cl_int err;

  cl_mem pipe = ::clCreatePipe(
    context->getRaw(),
    flags,
    size,
    qty,
    NULL,
    &err
  );

  CHECK_ERR(err);

  info.GetReturnValue().Set(NOCL_WRAP_AND_RELEASE(NoCLMem, pipe));
}


NAN_METHOD(GetPipeInfo) {
  Nan::HandleScope scope;
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(mem, NoCLMem, info[0]);

  // Arg 1
  cl_pipe_info param_name = info[1]->Uint32Value();

  switch(param_name) {
    case CL_PIPE_MAX_PACKETS:
    case CL_PIPE_PACKET_SIZE: {
      cl_uint val;
      CHECK_ERR(::clGetPipeInfo(mem->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      info.GetReturnValue().Set(JS_INT(val));
      return;
    }
  }

  return Nan::ThrowError(JS_STR(opencl::getExceptionMessage(CL_INVALID_VALUE)));
}

#endif

namespace Pipe {
NAN_MODULE_INIT(init)
{
#ifdef CL_VERSION_2_0
  Nan::SetMethod(target, "createPipe", CreatePipe);
  Nan::SetMethod(target, "getPipeInfo", GetPipeInfo);
#endif
}
} // namespace Pipe

} // namespace opencl
