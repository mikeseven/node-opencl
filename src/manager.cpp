#include "manager.h"

using namespace node;

namespace opencl {

void Manager::add(Persistent<Object>* p, cl_type id) {
  if(objects.count(p)<1) {
    objects[p]=id;
    references[p]=0;
  }
  references[p]++;
}

void Manager::remove(Persistent<Object>* p) {
  if(references.count(p)<1)
    return;
  if(--references[p]<=0) {
    objects.erase(p);
    references.erase(p);
  }
}

Persistent<Object>* Manager::find(cl_type id) {
  for(auto it=objects.begin();it!=objects.end();++it) {
    if(it->second == id) {
      return it->first;
    }
  }
  return nullptr;
}

void Manager::clear() {
  for(auto it=objects.begin();it!=objects.end();++it) {
    auto p= *(it->first);
    if(!p.IsEmpty() && !p.IsNearDeath()) {
      // TODO WebCLObject *obj=ObjectWrap::Unwrap<WebCLObject>(p);
      // obj->Destructor();
    }
  }
  references.clear();
  objects.clear();
}

void Manager::stats() {
  printf("OpenCL Manager stats:\n");
  printf("  #objects: %ld\n",objects.size());
  for(auto it=references.begin(); it!=references.end(); ++it) {
    auto p= *(it->first);
    if(!p.IsEmpty() && !p.IsNearDeath()) {
      // TODO WebCLObject *obj=ObjectWrap::Unwrap<WebCLObject>(p);
      // int count=it->second;
      // printf("    %s: %d\n",obj->getCLObjName(),count);
    }
  }
}

} // namespace webcl
