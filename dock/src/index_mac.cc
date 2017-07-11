/*
* osx index
*/

#include <nan.h>

namespace dock {

  void HelloMac(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // first parameter
    v8::String::Utf8Value cmd(info[0]);
    std::string name = std::string(*cmd);
    // return back rect
    std::string ret = "Hello " + name;
    info.GetReturnValue().Set(Nan::New(ret).ToLocalChecked());
  }

  void Init(v8::Local<v8::Object> exports) {
    exports -> Set(Nan::New("hello_mac").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(HelloMac) -> GetFunction());
  }

  NODE_MODULE(hello, Init);
}
