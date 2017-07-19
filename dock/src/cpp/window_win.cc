/*
* windows index
*/

#include <nan.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <windows.h>
#include <winuser.h>

namespace window_win {

  void LogLine(std::string text) {
    std::cout << text + "\n" << std::flush;
  }

  std::map<std::string, HWND> hwndMap;
  std::map<DWORD, HWINEVENTHOOK> hookMap;
  std::map<DWORD, v8::Local<v8::Function>> callbackMap;

  std::string converHwndToString(HWND hwnd) {
    std::stringstream ss;
    ss << hwnd;
    std::string str = ss.str();
    return str;
  }

  void out_findWindowHwnd(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    LPCTSTR className = (args[0]->IsNull() || args[0]->IsUndefined()) ? NULL : std::string(*arg0).c_str();
    // argument 1
    v8::String::Utf8Value arg1(args[1]);
    LPCTSTR windowName = (args[1]->IsNull() || args[1]->IsUndefined()) ? NULL : std::string(*arg1).c_str();
    // find
    HWND hwnd = FindWindow(className, windowName);
    // return
    std::string strHwnd = converHwndToString(hwnd);
    hwndMap[strHwnd] = hwnd;
    args.GetReturnValue().Set(Nan::New(strHwnd).ToLocalChecked());
  }

  void out_getForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // find
    HWND hwnd = GetForegroundWindow();
    // return
    std::string strHwnd = converHwndToString(hwnd);
    hwndMap[strHwnd] = hwnd;
    args.GetReturnValue().Set(Nan::New(strHwnd).ToLocalChecked());
  }

  void out_setForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    std::string strHwnd = std::string(*arg0);
    // apply
    HWND hwnd = hwndMap[strHwnd];
    BOOL setted = SetForegroundWindow(hwnd);
    // return
    args.GetReturnValue().Set(Nan::New(setted));
  }

  void out_getWindowRect(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    std::string strHwnd = std::string(*arg0);
    // get rect
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    RECT rect = { NULL };
    HWND hwnd = hwndMap[strHwnd];
    if (GetWindowRect(hwnd, &rect)) {
      left = rect.left;
      top = rect.top;
      right = rect.right;
      bottom = rect.bottom;
    }
    // return rect
    v8::Local<v8::Object> obj = v8::Object::New(isolate);
    obj->Set(Nan::New("left").ToLocalChecked(), Nan::New(left));
    obj->Set(Nan::New("top").ToLocalChecked(), Nan::New(top));
    obj->Set(Nan::New("right").ToLocalChecked(), Nan::New(right));
    obj->Set(Nan::New("bottom").ToLocalChecked(), Nan::New(bottom));
    args.GetReturnValue().Set(obj);
  }

  void out_isWindowVisible(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    std::string strHwnd = std::string(*arg0);
    // get
    HWND hwnd = hwndMap[strHwnd];
    BOOL visible = IsWindowVisible(hwnd);
    // return
    args.GetReturnValue().Set(Nan::New(visible));
  }

  void out_showWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    std::string strHwnd = std::string(*arg0);
    // get
    HWND hwnd = hwndMap[strHwnd];
    BOOL showed = ShowWindow(hwnd, 1);
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
    v8::Local<v8::Function> function = callbackMap[eventType];
    Nan::Callback callback(function);
    const unsigned argc = 2;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New(hWinEventHook),
      Nan::New(hwnd)
    };
    callback.Call(argc, argv);
  }

  void WrapSetWinEventHook(DWORD eventType) {
    if (hookMap.count(eventType) > 0) {
      return;
    }
    HWINEVENTHOOK hook = SetWinEventHook(eventType, eventType, NULL, WrapWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    hookMap[eventType] = hook;
  }

  void WrapUnhookWinEvent() {
    for(auto const &ent : hookMap) {
      auto const &value = ent.second;
      UnhookWinEvent(value);
    }
  }

  void out_unhookWinEvents(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    WrapUnhookWinEvent();
    args.GetReturnValue().Set(Nan::New(TRUE));
  }

  void out_setWinEventHookObjectHide(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(args[0]);
    DWORD eventType = EVENT_OBJECT_HIDE;
    callbackMap[eventType] = function;
    WrapSetWinEventHook(eventType);
  }

  void out_setWinEventHookObjectShow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(args[0]);
    DWORD eventType = EVENT_OBJECT_SHOW;
    callbackMap[eventType] = function;
    WrapSetWinEventHook(eventType);
  }

  void out_setWinEventHookLocationChange(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(args[0]);
    DWORD eventType = EVENT_OBJECT_LOCATIONCHANGE;
    callbackMap[eventType] = function;
    WrapSetWinEventHook(eventType);
  }

  void out_setWinEventHookMinimizeStart(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(args[0]);
    DWORD eventType = EVENT_SYSTEM_MINIMIZESTART;
    callbackMap[eventType] = function;
    WrapSetWinEventHook(eventType);
  }

  void out_setWinEventHookMinimizeEnd(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(args[0]);
    DWORD eventType = EVENT_SYSTEM_MINIMIZEEND;
    callbackMap[eventType] = function;
    WrapSetWinEventHook(eventType);
  }

  void out_setWinEventHookForeground(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(args[0]);
    DWORD eventType = EVENT_SYSTEM_FOREGROUND;
    callbackMap[eventType] = function;
    WrapSetWinEventHook(eventType);
  }

  void out_testCallback(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    v8::Local<v8::String> name = v8::Local<v8::String>::Cast(args[0]);
    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(args[1]);
    Nan::Callback callback(function);
    const unsigned argc = 3;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("hello").ToLocalChecked(),
      Nan::New("world").ToLocalChecked(),
      name,
    };
    callback.Call(argc, argv);
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
  }

  NODE_MODULE(dock_win, Init);

}
