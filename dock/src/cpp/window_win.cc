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
    v8::String::Utf8Value arg0(args[0]);
    auto className = args[0]->IsString() ? std::string(*arg0) : "";
    // argument 1
    v8::String::Utf8Value arg1(args[1]);
    auto windowName = args[1]->IsString() ? std::string(*arg1) : "";
    // find
    auto hwnd = FindWindow(className.c_str(), windowName.c_str());
    // return
    auto strHwnd = converHwndToString(hwnd);
    hwndMap[strHwnd] = hwnd;
    args.GetReturnValue().Set(Nan::New(strHwnd).ToLocalChecked());
  }

  void out_getForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // find
    auto hwnd = GetForegroundWindow();
    // return
    auto strHwnd = converHwndToString(hwnd);
    hwndMap[strHwnd] = hwnd;
    args.GetReturnValue().Set(Nan::New(strHwnd).ToLocalChecked());
  }

  void out_setForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // apply
    auto hwnd = hwndMap[strHwnd];
    auto setted = SetForegroundWindow(hwnd);
    // return
    args.GetReturnValue().Set(Nan::New(setted));
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

  void out_isWindowVisible(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // get
    auto hwnd = hwndMap[strHwnd];
    auto visible = IsWindowVisible(hwnd);
    // return
    args.GetReturnValue().Set(Nan::New(visible));
  }

  void out_showWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    // get
    auto hwnd = hwndMap[strHwnd];
    auto showed = ShowWindow(hwnd, 1);
    // return
    args.GetReturnValue().Set(Nan::New(showed));
  }

  void CALLBACK WrapWinEventProc(HWINEVENTHOOK hWinEventHook, DWORD eventType, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (callbackMap.count(eventType) == 0) {
      return;
    }
    std::string strHwnd = converHwndToString(hwnd);
    if (hwndMap.count(strHwnd) == 0) {
      return;
    }
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
    args.GetReturnValue().Set(Nan::New(TRUE));
  }

  void out_setWinEventHookObjectHide(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_OBJECT_HIDE;
    WrapSetWinEventHook(eventType, callback);
  }

  void out_setWinEventHookObjectShow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_OBJECT_SHOW;
    WrapSetWinEventHook(eventType, callback);
  }

  void out_setWinEventHookLocationChange(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_OBJECT_LOCATIONCHANGE;
    WrapSetWinEventHook(eventType, callback);
  }

  void out_setWinEventHookMinimizeStart(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_SYSTEM_MINIMIZESTART;
    WrapSetWinEventHook(eventType, callback);
  }

  void out_setWinEventHookMinimizeEnd(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_SYSTEM_MINIMIZEEND;
    WrapSetWinEventHook(eventType, callback);
  }

  void out_setWinEventHookForeground(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args);
    auto eventType = EVENT_SYSTEM_FOREGROUND;
    WrapSetWinEventHook(eventType, callback);
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
  }

  void out_destroy(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    WrapUnhookWinEvent();
    hwndMap.clear();
  }

  void Init(v8::Local<v8::Object> exports) {
    exports->Set(Nan::New("findWindowHwnd").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_findWindowHwnd)->GetFunction());
    exports->Set(Nan::New("getForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_getForegroundWindow)->GetFunction());
    exports->Set(Nan::New("setForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setForegroundWindow)->GetFunction());
    exports->Set(Nan::New("getWindowRect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_getWindowRect)->GetFunction());
    exports->Set(Nan::New("isWindowVisible").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_isWindowVisible)->GetFunction());
    exports->Set(Nan::New("showWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_showWindow)->GetFunction());
    // event hooks
    exports->Set(Nan::New("unhookWinEvents").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_unhookWinEvents)->GetFunction());
    exports->Set(Nan::New("setWinEventHookObjectHide").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookObjectHide)->GetFunction());
    exports->Set(Nan::New("setWinEventHookObjectShow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookObjectShow)->GetFunction());
    exports->Set(Nan::New("setWinEventHookLocationChange").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookLocationChange)->GetFunction());
    exports->Set(Nan::New("setWinEventHookMinimizeStart").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookMinimizeStart)->GetFunction());
    exports->Set(Nan::New("setWinEventHookMinimizeEnd").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookMinimizeEnd)->GetFunction());
    exports->Set(Nan::New("setWinEventHookForeground").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWinEventHookForeground)->GetFunction());
    // test
    exports->Set(Nan::New("testCallback").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_testCallback)->GetFunction());
    // destroy
    exports->Set(Nan::New("destroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_destroy)->GetFunction());
  }

  NODE_MODULE(dock_win, Init);

}
