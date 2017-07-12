/*
* osx index
*/

#include <nan.h>
#include <iostream>
#include <ApplicationServices/ApplicationServices.h>

namespace window_mac {

  void LogLine(std::string text) {
    std::cout << text + "\n" << std::flush;
  }

  CFArrayRef createWindowDescription(CGWindowID id) {
    CGWindowID idCArray[1] = { id };
    CFArrayRef idArray = CFArrayCreate(NULL, (const void **) idCArray, 1, NULL);
    CFArrayRef result = CGWindowListCreateDescriptionFromArray(idArray);
    CFRelease(idArray);
    return result;
  }

  void test() {
    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
    CFRelease(windowList);
  }

  void _helloMac(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // first parameter
    v8::String::Utf8Value cmd(info[0]);
    std::string name = std::string(*cmd);
    // return back rect
    std::string ret = "Hello " + name;
    LogLine(ret);
    info.GetReturnValue().Set(Nan::New(ret).ToLocalChecked());
  }

  void Init(v8::Local<v8::Object> exports) {
    exports -> Set(Nan::New("helloMac").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_helloMac) -> GetFunction());
  }

  NODE_MODULE(dock_mac, Init);
}
