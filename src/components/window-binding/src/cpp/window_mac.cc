/*
* osx index
*/

#include <nan.h>
#include <string>
#include <iostream>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

namespace window_mac {

  CFArrayRef createWindowDescription(CGWindowID id) {
    // NSWindow* nsWindow;
    CGWindowID idCArray[1] = { id };
    CFArrayRef idArray = CFArrayCreate(NULL, (const void **) idCArray, 1, NULL);
    CFArrayRef result = CGWindowListCreateDescriptionFromArray(idArray);
    CFRelease(idArray);
    return result;
  }

  void out_findWindowId(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    std::string ownerName;
    auto ownerNameIsString = args[0]->IsString();
    if (ownerNameIsString) {
      v8::String::Utf8Value arg0(args[0]);
      ownerName = std::string(*arg0);
    }
    // argument 1
    std::string windowName;
    auto windowNameIsString = args[1]->IsString();
    if (windowNameIsString) {
      v8::String::Utf8Value arg1(args[1]);
      windowName = std::string(*arg1);
    }
    // find
    int id = -1;
    if (ownerNameIsString || windowNameIsString) {
      // all windows
      CFArrayRef windowList = CGWindowListCopyWindowInfo(
        kCGWindowListExcludeDesktopElements, //kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements,
        kCGNullWindowID
      );
      CFIndex count = CFArrayGetCount(windowList);
      for (CFIndex i = 0; i < count; ++i) {
        CFDictionaryRef window = reinterpret_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(windowList, i));
        CFNumberRef window_id = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowNumber));
        CFNumberRef window_layer = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowLayer));
        if (!window_id || !window_layer) {
          continue;
        }
        // layer
        int layer;
        CFNumberGetValue(window_layer, kCFNumberIntType, &layer);
        if (layer != 0) {
          continue;
        }
        // owner
        bool ownerNameMatched = false;
        if (ownerNameIsString) {
          std::string owner;
          CFStringRef window_owner = reinterpret_cast<CFStringRef>(CFDictionaryGetValue(window, kCGWindowOwnerName));
          CFIndex ownerBufferSize = CFStringGetLength(window_owner) + 1;
          char ownerBuffer[ownerBufferSize];
          if (CFStringGetCString(window_owner, ownerBuffer, ownerBufferSize, kCFStringEncodingUTF8)) {
            owner = std::string(ownerBuffer);
          }
          ownerNameMatched = owner == ownerName;
        }
        // title
        bool windowNameMatched = false;
        if (windowNameIsString) {
          std::string title;
          CFStringRef window_title = reinterpret_cast<CFStringRef>(CFDictionaryGetValue(window, kCGWindowName));
          CFIndex titleBufferSize = CFStringGetLength(window_title) + 1;
          char titleBuffer[titleBufferSize];
          if (CFStringGetCString(window_title, titleBuffer, titleBufferSize, kCFStringEncodingUTF8)) {
            title = std::string(titleBuffer);
          }
          windowNameMatched = title == windowName;
        }
        // detect
        if (ownerNameMatched || windowNameMatched) {
          CFNumberGetValue(window_id, kCFNumberIntType, &id);
          break;
        }
      }
      CFRelease(windowList);
    }
    // ret
    args.GetReturnValue().Set(Nan::New(id));
  }

  void out_isWindowMinimized(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    CGWindowID id = args[0]->Int32Value();
    // get
    CFArrayRef idArray = CFArrayCreate(NULL, reinterpret_cast<const void **>(&id), 1, NULL);
    CFArrayRef windowArray = CGWindowListCreateDescriptionFromArray(idArray);
    bool minimized = false;
    if (windowArray && CFArrayGetCount(windowArray)) {
      CFDictionaryRef window = reinterpret_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(windowArray, 0));
      CFBooleanRef onScreen =  reinterpret_cast<CFBooleanRef>(CFDictionaryGetValue(window, kCGWindowIsOnscreen));
      minimized = (onScreen != kCFBooleanTrue);
    }
    CFRelease(idArray);
    CFRelease(windowArray);
    // return
    args.GetReturnValue().Set(Nan::New(minimized));
  }

  void out_helloWorld(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto name = std::string(*arg0);
    // return back
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
    // exports
    exports->Set(Nan::New("findWindowId").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_findWindowId)->GetFunction());
    exports->Set(Nan::New("isWindowMinimized").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_isWindowMinimized)->GetFunction());
    // test
    exports->Set(Nan::New("helloWorld").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_helloWorld)->GetFunction());
    exports->Set(Nan::New("testCallback").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_testCallback)->GetFunction());
    // destroy
    exports->Set(Nan::New("destroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_destroy)->GetFunction());
  }

  NODE_MODULE(wb, Init);
}
