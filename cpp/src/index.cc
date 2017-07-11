/*
* rcsfb index
*/

#include <nan.h>
#include "get_rect.h"

namespace rcsfb {

  void GetWindowRectCore(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // first parameter
    v8::String::Utf8Value cmd(info[0]);
    std::string threadName = std::string(*cmd);
    // get window rect
    int left = 0; int top = 0; int right = 0; int bottom = 0;
    rcsfb::get_window_rect(threadName, &left, &top, &right, &bottom);
    // return back rect
    std::string ret = std::to_string(left) + "|" + std::to_string(top) + "|" + std::to_string(right) + "|" + std::to_string(bottom);
    info.GetReturnValue().Set(Nan::New(ret).ToLocalChecked());
  }

  void Init(v8::Local<v8::Object> exports) {
    exports -> Set(Nan::New("getWindowRect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(GetWindowRectCore) -> GetFunction());
  }

  NODE_MODULE(hello, Init);

}
