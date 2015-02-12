#include "common.h"

namespace opencl {

/*
 * @return number of bytes per elements in a TypedArray
 * @return -1 if wrong type
 */
int getTypedArrayBytes(ExternalArrayType type)
{
  switch(type) {
    case kExternalByteArray:
    case kExternalUnsignedByteArray:
    case kExternalPixelArray:
      return 1;
    case kExternalShortArray:
    case kExternalUnsignedShortArray:
      return 2;
    case kExternalIntArray:
    case kExternalUnsignedIntArray:
    case kExternalFloatArray:
      return 4;
    case kExternalDoubleArray:
      return 8;
  }
  return -1;
}

void getPtrAndLen(const Local<Value> value, void* &ptr, int &len)
{
  ptr=NULL;
  len=0;
  if(!value->IsUndefined() && !value->IsNull()) {
    if(value->IsArray()) {
      Local<Array> arr=Local<Array>::Cast(value);
      ptr = arr->GetIndexedPropertiesExternalArrayData();
      len = arr->GetIndexedPropertiesExternalArrayDataLength() * getTypedArrayBytes(arr->GetIndexedPropertiesExternalArrayDataType());
    }
    else if(value->IsObject()) {
      Local<Object> obj=value->ToObject();
      String::AsciiValue name(obj->GetConstructorName());
      if(!strcmp("Buffer",*name)) {
        ptr=node::Buffer::Data(obj);
        len=(int) node::Buffer::Length(obj);
      }
      else {
        ptr = obj->GetIndexedPropertiesExternalArrayData();
        len = obj->GetIndexedPropertiesExternalArrayDataLength() * getTypedArrayBytes(obj->GetIndexedPropertiesExternalArrayDataType());
      }
    }
  }
}

}

// namespace opencl
