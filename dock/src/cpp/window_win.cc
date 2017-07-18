/*
* windows index
*/

#include <nan.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
// #include <windows.h>

namespace window_win {

  void LogLine(std::string text) {
    std::cout << text + "\n" << std::flush;
  }

  std::map<std::string, HWND> hwndCache;

  std::string converHwndToString(HWND hwnd) {
    std::stringstream ss;
    ss << hwnd;
    std::string str = ss.str();
    return str;
  }

  void _findWindowHwnd(const Nan::FunctionCallbackInfo<v8::Value>& args) {
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
    hwndCache[strHwnd] = hwnd;
    args.GetReturnValue().Set(Nan::New(strHwnd).ToLocalChecked());
  }

  void _getForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // find
    HWND hwnd = GetForegroundWindow();
    // return
    std::string strHwnd = converHwndToString(hwnd);
    hwndCache[strHwnd] = hwnd;
    args.GetReturnValue().Set(Nan::New(strHwnd).ToLocalChecked());
  }

  void _setForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    std::string strHwnd = std::string(*arg0);
    // apply
    HWND hwnd = hwndCache[strHwnd];
    BOOL setted = SetForegroundWindow(hwnd);
    // return
    args.GetReturnValue().Set(Nan::New(setted));
  }

  void _getWindowRect(const Nan::FunctionCallbackInfo<v8::Value>& args) {
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
    HWND hwnd = hwndCache[strHwnd];
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

  void _isWindowVisible(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    std::string strHwnd = std::string(*arg0);
    // get
    HWND hwnd = hwndCache[strHwnd];
    BOOL visible = IsWindowVisible(hwnd);
    // return
    args.GetReturnValue().Set(Nan::New(visible));
  }

  void _showWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    std::string strHwnd = std::string(*arg0);
    // get
    HWND hwnd = hwndCache[strHwnd];
    BOOL showed = ShowWindow(hwnd, 1);
    // return
    args.GetReturnValue().Set(Nan::New(showed));
  }

  // void _setWinEventHook(const Nan::FunctionCallbackInfo<v8::Value>& args) {
  // }

  // void _unhookWinEvent(const Nan::FunctionCallbackInfo<v8::Value>& args) {
  // }

  void _testCallback(const Nan::FunctionCallbackInfo<v8::Value>& args) {
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
    exports->Set(Nan::New("findWindowHwnd").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_findWindowHwnd)->GetFunction());
    exports->Set(Nan::New("getForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_getForegroundWindow)->GetFunction());
    exports->Set(Nan::New("setForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_setForegroundWindow)->GetFunction());
    exports->Set(Nan::New("getWindowRect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_getWindowRect)->GetFunction());
    exports->Set(Nan::New("isWindowVisible").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_isWindowVisible)->GetFunction());
    exports->Set(Nan::New("showWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_showWindow)->GetFunction());

    exports->Set(Nan::New("testCallback").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_testCallback)->GetFunction());
  }

  NODE_MODULE(dock_win, Init);

}
