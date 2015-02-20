#include "pipe.h"
#include "types.h"
#include "common.h"
#include <node_buffer.h>

using namespace node;

namespace opencl {

NAN_METHOD(CreatePipe) {
  NanScope();
  REQ_ARGS(5);

  // Arg 1
  NOCL_UNWRAP(context, NoCLContext, args[0]);

  // Arg 2
  cl_mem_flags flags = args[1]->Uint32Value();

  // Arg 2
  cl_uint size = args[2]->Uint32Value();

  // Arg 3
  cl_uint qty = args[3]->Uint32Value();

  // Arg 4
  if (!args[4]->IsNull()) {
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

  NanReturnValue(NOCL_WRAP(NoCLMem, pipe));
}


NAN_METHOD(GetPipeInfo) {
  NanScope();
  REQ_ARGS(2);

  // Arg 0
  NOCL_UNWRAP(mem, NoCLMem, args[0]);

  // Arg 1
  cl_pipe_info param_name = args[1]->Uint32Value();

  switch(param_name) {
    case CL_PIPE_MAX_PACKETS:
    case CL_PIPE_PACKET_SIZE: {
      cl_uint val;
      CHECK_ERR(::clGetPipeInfo(mem->getRaw(),param_name,sizeof(cl_uint), &val, NULL))
      NanReturnValue(JS_INT(val));
    }
  }

  return NanThrowError(JS_INT(CL_INVALID_VALUE));
}

namespace Pipe {
void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "createPipe", CreatePipe);
  NODE_SET_METHOD(exports, "getPipeInfo", GetPipeInfo);
}
} // namespace Pipe

} // namespace opencl
