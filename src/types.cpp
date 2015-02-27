#include "types.h"
#include "common.h"

NAN_METHOD(Equals) {
  NanScope();
  REQ_ARGS(1);

  v8::Handle<v8::External> data = v8::Handle<v8::External>::Cast(args.Data());

  NoCLObjectGen * self = static_cast<NoCLObjectGen *>(data->Value());

  if (!args[0]->IsObject()) {
    NanReturnValue(NanNew<Boolean>(false));
  }

  Local<Object> otherObj = args[0]->ToObject();
  NoCLObjectGen * other = static_cast<NoCLObjectGen *>(NanGetInternalFieldPointer(otherObj, 0));

  bool res = self->getUncastedRaw() == other->getUncastedRaw();

  NanReturnValue(NanNew<Boolean>(res));
}
