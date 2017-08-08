/*
* osx index
*/

#include <nan.h>
#include <string>
#include <iostream>
#include <map>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
// #include <Foundation/Foundation.h>
// #include <AppKit/NSRunningApplication.h>
// #include <AppKit/NSWorkspace.h>
#include <AvailabilityMacros.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

namespace window_mac {

  std::map<int, int> winId2Pid;
  std::map<int, int> winPid2Id;

  bool mouseEventRegistered = false;
  int previousEventPid;
  CFRunLoopSourceRef mouseEventSource;

  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> foregroundCallback;
  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> locationChangeCallback;

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
    int pid = -1;
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
        CFNumberRef window_pid =  reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowOwnerPID));
        CFNumberRef window_layer = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowLayer));
        if (!window_id || !window_pid || !window_layer) {
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
          CFNumberGetValue(window_pid, kCFNumberIntType, &pid);
          break;
        }
      }
      CFRelease(windowList);
    }
    // ret
    winId2Pid[id] = pid;
    winPid2Id[pid] = id;
    args.GetReturnValue().Set(Nan::New(id));
  }

  void out_getForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    bool setted = false;
    ProcessSerialNumber frontProcId;
    if (noErr == GetFrontProcess(&frontProcId)) {
      std::cout << "GetFrontProcess" << std::endl;
      setted = true;
    }
    // find
    int id = -1;
    int pid = -1;
    if (setted) {
      // all windows
      CFArrayRef windowList = CGWindowListCopyWindowInfo(
        kCGWindowListExcludeDesktopElements, //kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements,
        kCGNullWindowID
      );
      CFIndex count = CFArrayGetCount(windowList);
      for (CFIndex i = 0; i < count; ++i) {
        CFDictionaryRef window = reinterpret_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(windowList, i));
        CFNumberRef windowId = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowNumber));
        CFNumberRef windowPid =  reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowOwnerPID));
        if (windowId && windowPid) {
          ProcessSerialNumber procId;
          CFNumberGetValue(windowPid, kCFNumberIntType, &procId);
          // detect
          Boolean isSame;
          if (noErr == SameProcess(&frontProcId, &procId, &isSame) && !isSame) {
            CFNumberGetValue(windowId, kCFNumberIntType, &id);
            CFNumberGetValue(windowPid, kCFNumberIntType, &pid);
            break;
          }
        }
      }
      CFRelease(windowList);
    }
    // return
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
        std::cout << "SetFrontProcess" << std::endl;
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

  CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType eventType, CGEventRef event, void *refcon) {
    // https://developer.apple.com/documentation/coregraphics/1455885-cgeventgetintegervaluefield
    // https://developer.apple.com/documentation/coregraphics/cgeventfield?language=objc
    int eventPid = CGEventGetIntegerValueField(event, kCGEventTargetUnixProcessID);
    if (previousEventPid != eventPid) {
      previousEventPid = eventPid;
      if (!foregroundCallback.IsEmpty()) {
        auto foregroundIsolate = v8::Isolate::GetCurrent();
        auto foregroundCallbackLocal = v8::Local<v8::Function>::New(foregroundIsolate, foregroundCallback);
        auto windowId = winPid2Id[eventPid];
        const unsigned foregroundArgc = 1;
        v8::Local<v8::Value> foregroundArgv[foregroundArgc] = { Nan::New(windowId) };
        Nan::Callback foregroundCallbackLocalWrap(foregroundCallbackLocal);
        foregroundCallbackLocalWrap.Call(foregroundArgc, foregroundArgv);
      }
      std::cout << "Process Changed: " << eventPid << std::endl;
    }

    // https://developer.apple.com/documentation/coregraphics/cgeventflags?language=objc
    CGEventFlags flags = CGEventGetFlags(event);
    if ((flags & kCGEventFlagMaskShift) != 0) {
      // std::cout << "kCGEventFlagMaskShift" << std::endl;
    } else if ((flags & kCGEventFlagMaskControl) != 0) {
      // std::cout << "kCGEventFlagMaskControl" << std::endl;
    } else if ((flags & kCGEventFlagMaskCommand) != 0) {
      // std::cout << "kCGEventFlagMaskCommand" << std::endl;
    } else if ((flags & kCGEventFlagMaskAlternate) != 0) {
      // std::cout << "kCGEventFlagMaskAlternate" << std::endl;
    } else if ((flags & kCGEventFlagMaskAlphaShift) != 0) {
      // std::cout << "kCGEventFlagMaskAlphaShift" << std::endl;
    } else if ((flags & kCGEventFlagMaskNumericPad) != 0) {
      std::cout << "kCGEventFlagMaskNumericPad" << std::endl;
    }

    switch(eventType) {
      case kCGEventTapDisabledByTimeout: {
        std::cout << "kCGEventTapDisabledByTimeout" << std::endl;
        break;
      }
      case kCGEventTapDisabledByUserInput: {
        std::cout << "kCGEventTapDisabledByUserInput" << std::endl;
        break;
      }
      case kCGEventMouseMoved: {
        // std::cout << "kCGEventMouseMoved" << std::endl;
        break;
      }
      case kCGEventLeftMouseDown: {
        // std::cout << "kCGEventLeftMouseDown" << std::endl;
        break;
      }
      case kCGEventLeftMouseUp: {
        // std::cout << "kCGEventLeftMouseUp" << std::endl;
        break;
      }
      case kCGEventLeftMouseDragged: {
        if (winPid2Id.count(eventPid) != 0 && !locationChangeCallback.IsEmpty()) {
          auto locationChangeIsolate = v8::Isolate::GetCurrent();
          auto locationChangeCallbackLocal = v8::Local<v8::Function>::New(locationChangeIsolate, locationChangeCallback);
          auto windowId = winPid2Id[eventPid];
          const unsigned locationChangeArgc = 1;
          v8::Local<v8::Value> locationChangeArgv[locationChangeArgc] = { Nan::New(windowId) };
          Nan::Callback locationChangeCallbackLocalWrap(locationChangeCallbackLocal);
          locationChangeCallbackLocalWrap.Call(locationChangeArgc, locationChangeArgv);
        }
        // std::cout << "kCGEventLeftMouseDragged" << std::endl;
        break;
      }
      case kCGEventRightMouseDown: {
        // std::cout << "kCGEventRightMouseDown" << std::endl;
        break;
      }
      case kCGEventRightMouseUp: {
        // std::cout << "kCGEventRightMouseUp" << std::endl;
        break;
      }
      case kCGEventRightMouseDragged:  {
        // std::cout << "kCGEventRightMouseDragged" << std::endl;
        break;
      }
      case kCGEventKeyDown: {
        std::cout << "kCGEventKeyDown" << std::endl;
        break;
      }
      case kCGEventKeyUp: {
        std::cout << "kCGEventKeyUp" << std::endl;
        break;
      }
      default: {
        break;
      }
    }
    return event;
  }

  void StartEventHooks() {
    if (mouseEventRegistered) {
      return;
    }
    // https://developer.apple.com/documentation/coregraphics/cgeventtype?language=objc
    CGEventMask eventMask = (
      // https://developer.apple.com/documentation/coregraphics/cgeventmask?language=objc
      CGEventMaskBit(kCGEventMouseMoved) |
      CGEventMaskBit(kCGEventLeftMouseDragged) |
      CGEventMaskBit(kCGEventLeftMouseDown) |
      CGEventMaskBit(kCGEventLeftMouseUp) |
      CGEventMaskBit(kCGEventRightMouseDragged) |
      CGEventMaskBit(kCGEventRightMouseDown) |
      CGEventMaskBit(kCGEventRightMouseUp) |
      CGEventMaskBit(kCGEventKeyDown) |
      CGEventMaskBit(kCGEventKeyUp)
    );
    // https://developer.apple.com/documentation/coregraphics/1454426-cgeventtapcreate
    CFMachPortRef eventTap = CGEventTapCreate(
      kCGAnnotatedSessionEventTap, // https://developer.apple.com/documentation/coregraphics/cgeventtaplocation?language=objc
      kCGTailAppendEventTap, // https://developer.apple.com/documentation/coregraphics/cgeventtapplacement?language=objc
      kCGEventTapOptionDefault, // https://developer.apple.com/documentation/coregraphics/cgeventtapoptions?language=objc
      eventMask,
      CGEventCallback,
      NULL // refcon
    );
    // https://developer.apple.com/documentation/corefoundation/1400928-cfmachportcreaterunloopsource?language=objc
    CFRunLoopSourceRef eventSource = CFMachPortCreateRunLoopSource(
      kCFAllocatorDefault,
      eventTap,
      0
    );
    // https://developer.apple.com/documentation/corefoundation/1543356-cfrunloopaddsource?language=objc
    CFRunLoopAddSource(
      CFRunLoopGetMain(),
      eventSource,
      kCFRunLoopCommonModes
    );
    // set flag
    mouseEventSource = eventSource;
    mouseEventRegistered = true;
  }

  void UnhookEvents() {
    if (!mouseEventRegistered) {
      return;
    }
    // https://developer.apple.com/documentation/corefoundation/1542145-cfrunloopremovesource?language=objc
    CFRunLoopRemoveSource(
      CFRunLoopGetMain(),
      mouseEventSource,
      kCFRunLoopCommonModes
    );
    // set flag
    mouseEventRegistered = false;
  }

  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> GetCallback(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    auto arg0 = v8::Handle<v8::Function>::Cast(args[0]);
    v8::Persistent<v8::Function> callback(isolate, arg0);
    return callback;
  }

  void out_unhookWinEvents(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    UnhookEvents();
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookForeground(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    StartEventHooks();
    foregroundCallback = GetCallback(args);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookLocationChange(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    StartEventHooks();
    locationChangeCallback = GetCallback(args);
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
    UnhookEvents();
    std::cout << "destroy done" << std::endl;
  }

  void Init(v8::Local<v8::Object> exports) {
    // exports
    exports->Set(Nan::New("findWindowId").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_findWindowId)->GetFunction());
    exports->Set(Nan::New("getForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_getForegroundWindow)->GetFunction());
    exports->Set(Nan::New("setForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setForegroundWindow)->GetFunction());
    exports->Set(Nan::New("getWindowRect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_getWindowRect)->GetFunction());
    exports->Set(Nan::New("isWindowMinimized").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_isWindowMinimized)->GetFunction());
    // export event hooks
    exports->Set(Nan::New("unhookWinEvents").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_unhookWinEvents)->GetFunction());
    exports->Set(Nan::New("setWinEventHookForeground").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookForeground)->GetFunction());
    exports->Set(Nan::New("setWinEventHookLocationChange").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookLocationChange)->GetFunction());
    // test
    exports->Set(Nan::New("helloWorld").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_helloWorld)->GetFunction());
    exports->Set(Nan::New("testCallback").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_testCallback)->GetFunction());
    // destroy
    exports->Set(Nan::New("destroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_destroy)->GetFunction());
  }

  NODE_MODULE(wb, Init);
}
