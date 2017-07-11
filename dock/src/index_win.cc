/*
* windows index
*/

#include <nan.h>

namespace dock {

  void findWindowHwnd(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // parametera
    v8::String::Utf8Value cmd(info[0]);
    std::string className = std::string(*cmd);
    v8::String::Utf8Value cmd(info[1]);
    std::string windowName = std::string(*cmd);
    // find
    HWND hwnd = FindWindow(className, windowName);
    // return back rect
    std::string ret = std::to_string(hwnd);
    info.GetReturnValue().Set(Nan::New(ret).ToLocalChecked());
  }

  void Init(v8::Local<v8::Object> exports) {
    exports -> Set(Nan::New("findWindowHwnd").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(findWindowHwnd) -> GetFunction());
  }

  NODE_MODULE(hello, Init);

}
