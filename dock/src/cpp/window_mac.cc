/*
* osx index
*/

#include <nan.h>
#include <string>
#include <iostream>
#include <ApplicationServices/ApplicationServices.h>

namespace window_mac {

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

  void out_helloMac(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto name = std::string(*arg0);
    // return back rect
    auto ret = "Hello " + name;
    std::cout << "Message: " << ret << std::endl;
    args.GetReturnValue().Set(Nan::New(ret).ToLocalChecked());
  }

  void out_destroy(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    std::cout << "destroy" << std::endl;
  }

  void Init(v8::Local<v8::Object> exports) {
    exports->Set(Nan::New("helloMac").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_helloMac)->GetFunction());
    // destroy
    exports->Set(Nan::New("destroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_destroy)->GetFunction());
  }

  NODE_MODULE(dock_mac, Init);
}
