/*
* windows index
*/

#include <nan.h>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <windows.h>
#include <winuser.h>

namespace window_win {

  std::map<std::string, HWND> hwndMap;
  std::map<DWORD, HWINEVENTHOOK> hookMap;
  std::map<DWORD, v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>> callbackMap;

  std::string converHwndToString(HWND hwnd) {
    std::stringstream ss;
    ss << hwnd;
    std::string str = ss.str();
    return str;
  }

  void out_findWindowHwnd(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    std::string className;
    auto classNameIsString = args[0]->IsString();
    if (classNameIsString) {
      v8::String::Utf8Value arg0(args[0]);
      className = std::string(*arg0);
    }
    // argument 1
    std::string windowName;
    auto windowNameIsString = args[1]->IsString();
    if (windowNameIsString) {
      v8::String::Utf8Value arg1(args[1]);
      windowName = std::string(*arg1);
    }
    // find
    auto hwnd = FindWindow(
      classNameIsString ? className.c_str() : NULL,
      windowNameIsString ? windowName.c_str() : NULL
    );
    // return
    std::string strHwnd;
    if (hwnd != NULL) {
      strHwnd = converHwndToString(hwnd);
      hwndMap[strHwnd] = hwnd;
    }
    args.GetReturnValue().Set(Nan::New(strHwnd).ToLocalChecked());
  }

  void out_allowSetForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // query
    bool queried = false;
    DWORD processId;
    auto hwnd = hwndMap[strHwnd];
    if (hwnd) {
      GetWindowThreadProcessId(hwnd, &processId);
      queried = true;
    }
    // apply
    BOOL setted;
    if (queried && processId) {
      setted = AllowSetForegroundWindow(processId);
    } else {
      setted = AllowSetForegroundWindow(ASFW_ANY); // all processes
    }
    // return
    auto ret = (setted == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_setAlwaysOnTop(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    auto hwnd = hwndMap[strHwnd];
    // argument 1
    bool onTop = args[1]->BooleanValue();
    // apply
    BOOL setted;
    if (onTop) {
      setted = SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    } else {
      setted = SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    }
    // return
    auto ret = (setted == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_bringWindowToTop(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // apply
    auto hwnd = hwndMap[strHwnd];
    auto setted = BringWindowToTop(hwnd);
    // return
    auto ret = (setted == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));

    // SetWindowPos(windowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);  // it will bring window at the most front but makes it Always On Top.
    // SetWindowPos(windowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE); // just after above call, disable Always on Top.
  }

  void out_setForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // apply
    auto hwnd = hwndMap[strHwnd];
    auto setted = SetForegroundWindow(hwnd);
    // return
    auto ret = (setted == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_getWindowRect(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // get rect
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    RECT rect = { NULL };
    auto hwnd = hwndMap[strHwnd];
    if (GetWindowRect(hwnd, &rect)) {
      left = rect.left;
      top = rect.top;
      right = rect.right;
      bottom = rect.bottom;
    }
    // return rect
    auto isolate = args.GetIsolate();
    auto obj = v8::Object::New(isolate);
    obj->Set(Nan::New("left").ToLocalChecked(), Nan::New(left));
    obj->Set(Nan::New("top").ToLocalChecked(), Nan::New(top));
    obj->Set(Nan::New("right").ToLocalChecked(), Nan::New(right));
    obj->Set(Nan::New("bottom").ToLocalChecked(), Nan::New(bottom));
    args.GetReturnValue().Set(obj);
  }

  void out_setWindowPosition(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    auto hwnd = hwndMap[strHwnd];
    // argument 1
    auto x = args[1]->Int32Value();
    // argument 2
    auto y = args[2]->Int32Value();
    // apply
    auto setted = SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
    // return
    auto ret = (setted == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_isWindowVisible(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // get
    auto hwnd = hwndMap[strHwnd];
    auto visible = IsWindowVisible(hwnd);
    // return
    auto ret = (visible == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_showWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // get
    auto hwnd = hwndMap[strHwnd];
    auto showed = ShowWindow(hwnd, SW_SHOWNORMAL);
    // return
    auto ret = (showed == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_hideWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // get
    auto hwnd = hwndMap[strHwnd];
    auto showed = ShowWindow(hwnd, SW_HIDE);
    // return
    auto ret = (showed == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_isWindowMinimized(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // get
    auto hwnd = hwndMap[strHwnd];
    auto minimized = IsIconic(hwnd);
    // return
    auto ret = (minimized == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_minimizeWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // get
    auto hwnd = hwndMap[strHwnd];
    auto showed = ShowWindow(hwnd, SW_MINIMIZE);
    // return
    auto ret = (showed == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_restoreWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // get
    auto hwnd = hwndMap[strHwnd];
    auto showed = ShowWindow(hwnd, SW_RESTORE);
    // return
    auto ret = (showed == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void CALLBACK WrapWinEventProc(HWINEVENTHOOK hWinEventHook, DWORD eventType, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (callbackMap.count(eventType) == 0) {
      return;
    }
    std::string strHwnd = converHwndToString(hwnd);
    // if (hwndMap.count(strHwnd) == 0) {
    //   return;
    // }
    auto isolate = v8::Isolate::GetCurrent();
    auto function = callbackMap[eventType];
    auto funcLocal = v8::Local<v8::Function>::New(isolate, function);
    Nan::Callback callback(funcLocal);
    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New(strHwnd).ToLocalChecked()
    };
    callback.Call(argc, argv);
  }

  // https://msdn.microsoft.com/en-us/library/windows/desktop/dd318066(v=vs.85).aspx
  void WrapSetWinEventHook(DWORD eventType, v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> callback) {
    if (hookMap.count(eventType) > 0) {
      return;
    }
    callbackMap[eventType] = callback;
    HWINEVENTHOOK hook = SetWinEventHook(eventType, eventType, NULL, WrapWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    hookMap[eventType] = hook;
  }

  void WrapUnhookWinEvent() {
    for(auto const &ent : hookMap) {
      auto const &value = ent.second;
      UnhookWinEvent(value);
    }
    hookMap.clear();
    callbackMap.clear();
  }

  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> GetCallback(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    auto arg0 = v8::Handle<v8::Function>::Cast(args[0]);
    v8::Persistent<v8::Function> callback(isolate, arg0);
    return callback;
  }

  void out_unhookWinEvents(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    WrapUnhookWinEvent();
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectCreate(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_OBJECT_CREATE;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectDestroy(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_OBJECT_DESTROY;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectHide(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_OBJECT_HIDE;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectShow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_OBJECT_SHOW;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookLocationChange(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_OBJECT_LOCATIONCHANGE;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookMinimizeStart(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_SYSTEM_MINIMIZESTART;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookMinimizeEnd(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_SYSTEM_MINIMIZEEND;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookForeground(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_SYSTEM_FOREGROUND;
    WrapSetWinEventHook(eventType, callback);
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
    WrapUnhookWinEvent();
    hwndMap.clear();
    std::cout << "destroy done" << std::endl;
  }

  void Init(v8::Local<v8::Object> exports) {
    // exports
    exports->Set(Nan::New("findWindowHwnd").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_findWindowHwnd)->GetFunction());
    exports->Set(Nan::New("allowSetForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_allowSetForegroundWindow)->GetFunction());
    exports->Set(Nan::New("setAlwaysOnTop").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setAlwaysOnTop)->GetFunction());
    exports->Set(Nan::New("bringWindowToTop").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_bringWindowToTop)->GetFunction());
    exports->Set(Nan::New("setForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setForegroundWindow)->GetFunction());
    exports->Set(Nan::New("getWindowRect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_getWindowRect)->GetFunction());
    exports->Set(Nan::New("setWindowPosition").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWindowPosition)->GetFunction());
    exports->Set(Nan::New("isWindowVisible").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_isWindowVisible)->GetFunction());
    exports->Set(Nan::New("showWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_showWindow)->GetFunction());
    exports->Set(Nan::New("hideWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_hideWindow)->GetFunction());
    exports->Set(Nan::New("isWindowMinimized").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_isWindowMinimized)->GetFunction());
    exports->Set(Nan::New("minimizeWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_minimizeWindow)->GetFunction());
    exports->Set(Nan::New("restoreWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_restoreWindow)->GetFunction());
    // export event hooks
    exports->Set(Nan::New("unhookWinEvents").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_unhookWinEvents)->GetFunction());
    exports->Set(Nan::New("setWinEventHookObjectCreate").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookObjectCreate)->GetFunction());
    exports->Set(Nan::New("setWinEventHookObjectDestroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookObjectDestroy)->GetFunction());
    exports->Set(Nan::New("setWinEventHookObjectHide").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookObjectHide)->GetFunction());
    exports->Set(Nan::New("setWinEventHookObjectShow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookObjectShow)->GetFunction());
    exports->Set(Nan::New("setWinEventHookLocationChange").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookLocationChange)->GetFunction());
    exports->Set(Nan::New("setWinEventHookMinimizeStart").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookMinimizeStart)->GetFunction());
    exports->Set(Nan::New("setWinEventHookMinimizeEnd").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookMinimizeEnd)->GetFunction());
    exports->Set(Nan::New("setWinEventHookForeground").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookForeground)->GetFunction());
    // test
    exports->Set(Nan::New("helloWorld").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_helloWorld)->GetFunction());
    exports->Set(Nan::New("testCallback").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_testCallback)->GetFunction());
    // destroy
    exports->Set(Nan::New("destroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_destroy)->GetFunction());
  }

  NODE_MODULE(wb, Init);

}
