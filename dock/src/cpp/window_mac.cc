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

  void out_helloWorld(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto name = std::string(*arg0);
    // return back rect
    auto ret = "Hello " + name;
    std::cout << "Message: " << ret << std::endl;
    args.GetReturnValue().Set(Nan::New(ret).ToLocalChecked());
  }

  void out_testCallback(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto name = v8::Local<v8::String>::Cast(args[0]);
    auto function = v8::Local<v8::Function>::Cast(args[1]);
    Nan::Callback callback(function);
    const unsigned argc = 3;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("hello").ToLocalChecked(),
      Nan::New("world").ToLocalChecked(),
      name,
    };
    callback.Call(argc, argv);
    std::cout << "callback done" << std::endl;
  }

  void out_destroy(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    std::cout << "destroy done" << std::endl;
  }

  void Init(v8::Local<v8::Object> exports) {
    // test
    exports->Set(Nan::New("helloWorld").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_helloWorld)->GetFunction());
    exports->Set(Nan::New("testCallback").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_testCallback)->GetFunction());
    // destroy
    exports->Set(Nan::New("destroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_destroy)->GetFunction());
  }

  NODE_MODULE(dock_mac, Init);
}
