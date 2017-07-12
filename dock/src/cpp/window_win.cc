/*
* windows index
*/

#include <nan.h>
#include <string>
#include <sstream>
#include <map>
// #include <windows.h>
// #include <tlhelp32.h>
// #include <tchar.h>

namespace window_win {

  std::map<std::string, HWND> hwndCache;

  std::string converHwndToString(HWND hwnd) {
    std::stringstream ss;
    ss << hwnd;
    std::string str = ss.str();
    return str;
  }

  void _findWindowHwnd(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // parameter 0
    v8::String::Utf8Value cmd0(info[0]);
    LPCTSTR className = (std::string(*cmd0) == "null") ? NULL : std::string(*cmd0).c_str();
    // parameter 1
    v8::String::Utf8Value cmd1(info[1]);
    LPCTSTR windowName = (std::string(*cmd1) == "null") ? NULL : std::string(*cmd1).c_str();
    // find
    HWND hwnd = FindWindow(className, windowName);
    // _tprintf( TEXT("\n  found     = %p"), hwnd );
    // return
    std::string strHwnd = converHwndToString(hwnd);
    hwndCache[strHwnd] = hwnd;
    info.GetReturnValue().Set(Nan::New(strHwnd).ToLocalChecked());
  }

  void _getForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // find
    HWND hwnd = GetForegroundWindow();
    // return
    std::string strHwnd = converHwndToString(hwnd);
    hwndCache[strHwnd] = hwnd;
    info.GetReturnValue().Set(Nan::New(strHwnd).ToLocalChecked());
  }

  void _setForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // parameter 0
    v8::String::Utf8Value cmd(info[0]);
    std::string strHwnd = std::string(*cmd);
    // apply
    HWND hwnd = hwndCache[strHwnd];
    BOOL setted = SetForegroundWindow(hwnd);
    // return
    info.GetReturnValue().Set(Nan::New(setted));
  }

  void _getWindowRect(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // parameter 0
    v8::String::Utf8Value cmd(info[0]);
    std::string strHwnd = std::string(*cmd);
    // _tprintf( TEXT("\n  str input     = %s"), strHwnd.c_str() );
    // get rect
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    RECT rect = { NULL };
    HWND hwnd = hwndCache[strHwnd];
    // _tprintf( TEXT("\n  hwnd converd     = %p"), hwnd );
    if (GetWindowRect(hwnd, &rect)) {
      left = rect.left;
      top = rect.top;
      right = rect.right;
      bottom = rect.bottom;
    }
    // return rect
    std::string ret = std::to_string(left) + "|" + std::to_string(top) + "|" + std::to_string(right) + "|" + std::to_string(bottom);
    info.GetReturnValue().Set(Nan::New(ret).ToLocalChecked());
  }

  void _isWindowVisible(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // parameter 0
    v8::String::Utf8Value cmd(info[0]);
    std::string strHwnd = std::string(*cmd);
    // get
    HWND hwnd = hwndCache[strHwnd];
    BOOL visible = IsWindowVisible(hwnd);
    // return
    info.GetReturnValue().Set(Nan::New(visible));
  }

  void _showWindow(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    // parameter 0
    v8::String::Utf8Value cmd(info[0]);
    std::string strHwnd = std::string(*cmd);
    // get
    HWND hwnd = hwndCache[strHwnd];
    BOOL showed = ShowWindow(hwnd, 1);
    // return
    info.GetReturnValue().Set(Nan::New(showed));
  }

  // void _setWinEventHook(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // }

  // void _unhookWinEvent(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // }

  void Init(v8::Local<v8::Object> exports) {
    exports -> Set(Nan::New("findWindowHwnd").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_findWindowHwnd) -> GetFunction());
    exports -> Set(Nan::New("getForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_getForegroundWindow) -> GetFunction());
    exports -> Set(Nan::New("setForegroundWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_setForegroundWindow) -> GetFunction());
    exports -> Set(Nan::New("getWindowRect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_getWindowRect) -> GetFunction());
    exports -> Set(Nan::New("isWindowVisible").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_isWindowVisible) -> GetFunction());
    exports -> Set(Nan::New("showWindow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(_showWindow) -> GetFunction());
  }

  NODE_MODULE(dock_win, Init);

}
