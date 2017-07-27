/*
* osx index
*/

#include <nan.h>
#include <string>
#include <iostream>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
// #include <Foundation/Foundation.h>
// #include <AppKit/NSRunningApplication.h>
// #include <AppKit/NSWorkspace.h>
#include <AvailabilityMacros.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

namespace window_mac {

  bool mouseEventRegistered = false;
  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> function;

  // void test() {
  //   int id = 0;
  //   auto app = AXUIElementCreateApplication(id);

  //   CGEventRef event = CGEventCreate(nil);
  //   CGPoint loc = CGEventGetLocation(event);
  //   CFRelease(event);
  // }

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

  void out_setForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    CGWindowID windowId = args[0]->Int32Value();
    // get proc id
    ProcessSerialNumber procId;
    CFArrayRef idArray = CFArrayCreate(NULL, reinterpret_cast<const void **>(&windowId), 1, NULL);
    CFArrayRef windowArray = CGWindowListCreateDescriptionFromArray(idArray);
    if (windowArray && CFArrayGetCount(windowArray)) {
      CFDictionaryRef window = reinterpret_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(windowArray, 0));
      CFNumberRef ownerPid =  reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowOwnerPID));
      if (ownerPid) {
        CFNumberGetValue(ownerPid, kCFNumberIntType, &procId);
      }
    }
    CFRelease(idArray);
    CFRelease(windowArray);
    // apply
    bool setted = false;
    ProcessSerialNumber frontProcId;
    if (noErr == GetFrontProcess(&frontProcId)) {
      Boolean isSame;
      if (noErr == SameProcess(&frontProcId, &procId, &isSame) && !isSame) {
        SetFrontProcess(&procId);
        setted = true;
      }
    }
    // return
    args.GetReturnValue().Set(Nan::New(setted));
  }

  void out_getWindowRect(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    CGWindowID windowId = args[0]->Int32Value();
    // get rect
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    CFArrayRef idArray = CFArrayCreate(NULL, reinterpret_cast<const void **>(&windowId), 1, NULL);
    CFArrayRef windowArray = CGWindowListCreateDescriptionFromArray(idArray);
    if (windowArray && CFArrayGetCount(windowArray)) {
      CFDictionaryRef window = reinterpret_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(windowArray, 0));
      CFDictionaryRef windowBounds = reinterpret_cast<CFDictionaryRef>(CFDictionaryGetValue(window, kCGWindowBounds));
      CGRect windowRect;
      if (windowBounds && CGRectMakeWithDictionaryRepresentation(windowBounds, &windowRect)) {
        left = windowRect.origin.x;
        top = windowRect.origin.y;
        right = windowRect.origin.x + windowRect.size.width;
        bottom = windowRect.origin.y + windowRect.size.height;
      }
    }
    CFRelease(idArray);
    CFRelease(windowArray);
    // return rect
    auto isolate = args.GetIsolate();
    auto obj = v8::Object::New(isolate);
    obj->Set(Nan::New("left").ToLocalChecked(), Nan::New(left));
    obj->Set(Nan::New("top").ToLocalChecked(), Nan::New(top));
    obj->Set(Nan::New("right").ToLocalChecked(), Nan::New(right));
    obj->Set(Nan::New("bottom").ToLocalChecked(), Nan::New(bottom));
    args.GetReturnValue().Set(obj);
  }

  void out_isWindowMinimized(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    CGWindowID windowId = args[0]->Int32Value();
    // get
    CFArrayRef idArray = CFArrayCreate(NULL, reinterpret_cast<const void **>(&windowId), 1, NULL);
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

  CGEventRef CGEventCallback(CGEventTapProxy Proxy, CGEventType Type, CGEventRef Event, void *Refcon) {
    switch(Type) {
      case kCGEventLeftMouseDragged:
      case kCGEventRightMouseDragged: {
        auto isolate = v8::Isolate::GetCurrent();
        auto funcLocal = v8::Local<v8::Function>::New(isolate, function);
        Nan::Callback callback(funcLocal);
        const unsigned argc = 1;
        v8::Local<v8::Value> argv[argc] = {
          Nan::New(1)
        };
        callback.Call(argc, argv);
        break;
      }
      default: {
        break;
      }
    }
    return Event;
  }

  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> GetCallback(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    auto arg0 = v8::Handle<v8::Function>::Cast(args[0]);
    v8::Persistent<v8::Function> callback(isolate, arg0);
    return callback;
  }

  void out_setMouseDragEvent(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    if (!mouseEventRegistered) {
      auto eventMask = ((1 << kCGEventLeftMouseDragged) | (1 << kCGEventRightMouseDragged));
      auto eventTap = CGEventTapCreate(
        kCGSessionEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault,
        eventMask,
        CGEventCallback,
        NULL
      );
      CFRunLoopAddSource(
        CFRunLoopGetMain(),
        CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0),
        kCFRunLoopCommonModes
      );
      mouseEventRegistered = true;
    }
    //
    function = GetCallback(args);
    args.GetReturnValue().Set(Nan::New(true));
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
    exports->Set(Nan::New("setForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setForegroundWindow)->GetFunction());
    exports->Set(Nan::New("getWindowRect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_getWindowRect)->GetFunction());
    exports->Set(Nan::New("isWindowMinimized").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_isWindowMinimized)->GetFunction());
    // events
    exports->Set(Nan::New("setMouseDragEvent").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setMouseDragEvent)->GetFunction());
    // test
    exports->Set(Nan::New("helloWorld").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_helloWorld)->GetFunction());
    exports->Set(Nan::New("testCallback").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_testCallback)->GetFunction());
    // destroy
    exports->Set(Nan::New("destroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_destroy)->GetFunction());
  }

  NODE_MODULE(wb, Init);
}
