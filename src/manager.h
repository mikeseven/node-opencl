#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "common.h"
#include <map>

using namespace v8;

//namespace opencl {
//
//typedef void* cl_type;
//
//class Manager {
//public:
//  static Manager* instance() {
//    static Manager mgr;
//    return &mgr;
//  }
//
////  void add(Persistent<Object>* p, cl_type id);
////  void remove(Persistent<Object>* p);
////  Persistent<Object>* find(cl_type id);
////  void clear();
////  void stats();
//
//private:
//  explicit Manager() {}
//  ~Manager() {
//#ifdef LOGGING
//    cout<<"~Manager"<<endl;
//#endif
//    clear();
//  }
//
//private:
//  map<Persistent<Object>*, cl_type> objects;
//  map<Persistent<Object>*, int> references;
//};
//
//} // namespace webcl

#endif // __MANAGER_H__
