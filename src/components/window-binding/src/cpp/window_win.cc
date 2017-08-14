/*
* windows index
*/

#include <nan.h>
#include <string>
#include <iostream>
#include <sstream>
#include <map> // http://en.cppreference.com/w/cpp/container/map
#include <list> // http://en.cppreference.com/w/cpp/container/list
#include <tuple> // http://en.cppreference.com/w/cpp/utility/pair/pair
#include <vector> // http://en.cppreference.com/w/cpp/container/vector
#include <windows.h>
#include <winuser.h>
#include <UIAutomation.h>
#include <UIAutomationClient.h>
#include <comdef.h>
// #include <OleAcc.h>
// #include <oleauto.h>
// #include <cassert>

namespace window_win {

  std::map<std::string, HWND> hwndMap;

  std::string converHwndToString(HWND hwnd) {
    std::stringstream ss;
    ss << hwnd;
    std::string str = ss.str();
    return str;
  }

  HWND getHwndArg(const Nan::FunctionCallbackInfo<v8::Value>& args, int index) {
    if (args[index]->IsUndefined() || args[index]->IsNull()) {
      return NULL;
    }
    v8::String::Utf8Value arg(args[index]);
    auto strHwnd = std::string(*arg);
    if (hwndMap.count(strHwnd) > 0) {
      auto hwnd = hwndMap[strHwnd];
      return hwnd;
    } else {
      return NULL;
    }
  }

  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> getCallbackArg(const Nan::FunctionCallbackInfo<v8::Value>& args, int index) {
    auto isolate = args.GetIsolate();
    auto arg = v8::Handle<v8::Function>::Cast(args[index]);
    v8::Persistent<v8::Function> callback(isolate, arg);
    return callback;
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
    auto hwnd = getHwndArg(args, 0);
    // query
    bool queried = false;
    DWORD processId;
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
    auto hwnd = getHwndArg(args, 0);
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
    auto hwnd = getHwndArg(args, 0);
    // apply
    auto setted = BringWindowToTop(hwnd);
    // return
    auto ret = (setted == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));

    // SetWindowPos(windowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);  // it will bring window at the most front but makes it Always On Top.
    // SetWindowPos(windowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE); // just after above call, disable Always on Top.
  }

  void out_setForegroundWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // apply
    auto setted = SetForegroundWindow(hwnd);
    // return
    auto ret = (setted == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_getWindowRect(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // get rect
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    RECT rect = { NULL };
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

  void out_setWindowRect(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument hwnd
    auto hwnd = getHwndArg(args, 0);
    // argument rect
    auto left = args[1]->Int32Value();
    auto top = args[3]->Int32Value();
    auto right = args[2]->Int32Value();
    auto bottom = args[4]->Int32Value();
    // apply
    UINT flags = SWP_NOOWNERZORDER;
    if (left == -32000 || top == -32000) {
      flags = flags | SWP_NOMOVE;
    }
    if (right < 0 || bottom < 0) {
      flags = flags | SWP_NOSIZE;
    }
    auto setted = SetWindowPos(hwnd, NULL, left, top, right - left, bottom - top, flags);
    // return
    auto ret = (setted == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_isWindowVisible(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // get
    auto visible = IsWindowVisible(hwnd);
    // return
    auto ret = (visible == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_showWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // get
    auto showed = ShowWindow(hwnd, SW_SHOWNORMAL);
    // return
    auto ret = (showed == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_hideWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // get
    auto showed = ShowWindow(hwnd, SW_HIDE);
    // return
    auto ret = (showed == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_isWindowMinimized(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // get
    auto minimized = IsIconic(hwnd);
    // return
    auto ret = (minimized == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_minimizeWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // get
    auto showed = ShowWindow(hwnd, SW_MINIMIZE);
    // return
    auto ret = (showed == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  void out_restoreWindow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // get
    auto showed = ShowWindow(hwnd, SW_RESTORE);
    // return
    auto ret = (showed == TRUE);
    args.GetReturnValue().Set(Nan::New(ret));
  }

  /****************************************** events start ************************************************/

  std::map<DWORD, std::list<std::pair<HWND, v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>>>> persistentMapNoHwnd;
  std::map<DWORD, std::list<std::pair<HWND, v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>>>> persistentMapWithHwnd;
  std::map<DWORD, std::list<HWINEVENTHOOK>> hookMapNoHwnd;
  std::map<DWORD, std::list<HWINEVENTHOOK>> hookMapWithHwnd;

  void CALLBACK winEventProcNoHwnd(HWINEVENTHOOK hWinEventHook, DWORD eventType, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    auto isolate = v8::Isolate::GetCurrent();
    auto strHwnd = converHwndToString(hwnd);
    auto persistents = persistentMapNoHwnd[eventType];
    for (auto item : persistents) {
      auto function = item.second;
      auto funcLocal = v8::Local<v8::Function>::New(isolate, function);
      Nan::Callback callback(funcLocal);
      const unsigned argc = 1;
      v8::Local<v8::Value> argv[argc] = {
        Nan::New(strHwnd).ToLocalChecked()
      };
      callback.Call(argc, argv);
    }
  }

  void CALLBACK winEventProcWithHwnd(HWINEVENTHOOK hWinEventHook, DWORD eventType, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    auto isolate = v8::Isolate::GetCurrent();
    auto strHwnd = converHwndToString(hwnd);
    auto persistents = persistentMapWithHwnd[eventType];
    for (auto item : persistents) {
      if (item.first != hwnd) {
        continue;
      }
      auto function = item.second;
      auto funcLocal = v8::Local<v8::Function>::New(isolate, function);
      Nan::Callback callback(funcLocal);
      const unsigned argc = 1;
      v8::Local<v8::Value> argv[argc] = {
        Nan::New(strHwnd).ToLocalChecked()
      };
      callback.Call(argc, argv);
    }
  }

  // https://msdn.microsoft.com/en-us/library/windows/desktop/dd318066(v=vs.85).aspx
  void setWinEventHookWrap(DWORD eventType, HWND hwnd, v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> callback) {
    // cache
    std::pair<HWND, v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>> p(hwnd, callback);
    // register
    if (hwnd == NULL) {
      std::cout << "setWinEventHook hwnd: NULL" << std::endl;
      persistentMapNoHwnd[eventType].push_back(p);
      if (hookMapNoHwnd.count(eventType) > 0) { return; } // check hook
      HWINEVENTHOOK hook = SetWinEventHook(eventType, eventType, NULL, winEventProcNoHwnd, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
      hookMapNoHwnd[eventType].push_back(hook);
    } else {
      std::cout << "setWinEventHook hwnd: " << hwnd << std::endl;
      persistentMapWithHwnd[eventType].push_back(p);
      if (hookMapWithHwnd.count(eventType) > 0) { return; } // check hook
      DWORD dwProcessId;
      DWORD dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);
      HWINEVENTHOOK hook = SetWinEventHook(eventType, eventType, NULL, winEventProcWithHwnd, dwProcessId, dwThreadId, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
      hookMapWithHwnd[eventType].push_back(hook);
    }
  }

  void unhookWinEventWrap() {
    for(auto const &ent : hookMapNoHwnd) {
      auto const &hooks = ent.second;
      for (auto hook : hooks) {
        UnhookWinEvent(hook);
      }
    }
    for(auto const &ent : hookMapWithHwnd) {
      auto const &hooks = ent.second;
      for (auto hook : hooks) {
        UnhookWinEvent(hook);
      }
    }
    hookMapNoHwnd.clear();
    hookMapWithHwnd.clear();
    persistentMapNoHwnd.clear();
    persistentMapWithHwnd.clear();
  }

  void out_unhookWinEvents(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    unhookWinEventWrap();
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectCreate(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto hwnd = getHwndArg(args, 0);
    auto callback = getCallbackArg(args, 1);
    auto eventType = EVENT_OBJECT_CREATE;
    setWinEventHookWrap(eventType, hwnd, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectDestroy(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto hwnd = getHwndArg(args, 0);
    auto callback = getCallbackArg(args, 1);
    auto eventType = EVENT_OBJECT_DESTROY;
    setWinEventHookWrap(eventType, hwnd, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectHide(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto hwnd = getHwndArg(args, 0);
    auto callback = getCallbackArg(args, 1);
    auto eventType = EVENT_OBJECT_HIDE;
    setWinEventHookWrap(eventType, hwnd, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectShow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto hwnd = getHwndArg(args, 0);
    auto callback = getCallbackArg(args, 1);
    auto eventType = EVENT_OBJECT_SHOW;
    setWinEventHookWrap(eventType, hwnd, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookLocationChange(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto hwnd = getHwndArg(args, 0);
    auto callback = getCallbackArg(args, 1);
    auto eventType = EVENT_OBJECT_LOCATIONCHANGE;
    setWinEventHookWrap(eventType, hwnd, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookMinimizeStart(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto hwnd = getHwndArg(args, 0);
    auto callback = getCallbackArg(args, 1);
    auto eventType = EVENT_SYSTEM_MINIMIZESTART;
    setWinEventHookWrap(eventType, hwnd, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookMinimizeEnd(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto hwnd = getHwndArg(args, 0);
    auto callback = getCallbackArg(args, 1);
    auto eventType = EVENT_SYSTEM_MINIMIZEEND;
    setWinEventHookWrap(eventType, hwnd, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookForeground(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto hwnd = getHwndArg(args, 0);
    auto callback = getCallbackArg(args, 1);
    auto eventType1 = EVENT_SYSTEM_FOREGROUND;
    setWinEventHookWrap(eventType1, hwnd, callback);
    auto eventType2 = EVENT_SYSTEM_CAPTURESTART;
    setWinEventHookWrap(eventType2, hwnd, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  /****************************************** events end ************************************************/

  /****************************************** automation start ************************************************/

  class CustomAutomationEventHandler: public IUIAutomationEventHandler {

    private: LONG _refCount;
    public: int _eventCount;
    private: v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> callbackFunc;

    public: CustomAutomationEventHandler(v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> cb): _refCount(1), _eventCount(0) {
      callbackFunc = cb;
    }

    ULONG STDMETHODCALLTYPE AddRef() {
      ULONG ret = InterlockedIncrement(&_refCount);
      return ret;
    }

    ULONG STDMETHODCALLTYPE Release() {
      ULONG ret = InterlockedDecrement(&_refCount);
      if (ret == 0) {
        delete this;
        return 0;
      }
      return ret;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppInterface) {
      if (riid == __uuidof(IUnknown)) {
        *ppInterface = static_cast<IUIAutomationEventHandler*>(this);
      } else if (riid == __uuidof(IUIAutomationEventHandler)) {
        *ppInterface = static_cast<IUIAutomationEventHandler*>(this);
      } else {
        *ppInterface = NULL;
        return E_NOINTERFACE;
      }
      this->AddRef();
      return S_OK;
    }

    HRESULT STDMETHODCALLTYPE HandleAutomationEvent(IUIAutomationElement* sender, EVENTID eventId) {
      _eventCount++;
      switch (eventId) {
        case UIA_Invoke_InvokedEventId:
          std::cout << "automation event UIA_Invoke_InvokedEventId" << std::endl;
          break;
        case UIA_StructureChangedEventId:
          std::cout << "automation event UIA_StructureChangedEventId" << std::endl;
          break;
        default:
          std::cout << "automation event " << eventId << std::endl;
          break;
      }
      // callback
      auto isolate = v8::Isolate::GetCurrent();
      auto funcLocal = v8::Local<v8::Function>::New(isolate, callbackFunc);
      Nan::Callback callback(funcLocal);
      const unsigned argc = 1;
      v8::Local<v8::Value> argv[argc] = {
        Nan::New("abc").ToLocalChecked()
      };
      callback.Call(argc, argv);
      // ret
      return S_OK;
    }
  };

  IUIAutomation* automation = nullptr;
  CustomAutomationEventHandler* eventHandler = nullptr;

  std::string bstr2str(BSTR source){
    //source = L"lol2inside";
    _bstr_t wrapped_bstr = _bstr_t(source);
    int length = wrapped_bstr.length();
    char* char_array = new char[length];
    strcpy_s(char_array, length+1, wrapped_bstr);
    return char_array;
  }

  BSTR str2bstr(std::string str) {
    std::wstring stemp = std::wstring(str.begin(), str.end());
    auto ret = SysAllocString(stemp.c_str());
    return ret;
  }

  int getElementArrayLength(IUIAutomationElementArray* elements) {
    if (elements) {
      int length = 0;
      if (SUCCEEDED(elements->get_Length(&length)))
        return length;
    }
    return 0;
  }

  // https://msdn.microsoft.com/en-us/library/windows/desktop/ee684017(v=vs.85).aspx
  IUIAutomationCondition* BuildListViewCondition() {
    // ClassName
    VARIANT classNameProperty;
    classNameProperty.vt = VT_BSTR;
    classNameProperty.bstrVal = str2bstr("NetUIVirtualListView");
    IUIAutomationCondition* classNamecondition = nullptr;
    automation->CreatePropertyCondition(UIA_ClassNamePropertyId, classNameProperty, &classNamecondition);
    // AutomationId
    VARIANT automationIdProperty;
    automationIdProperty.vt = VT_BSTR;
    automationIdProperty.bstrVal = str2bstr("idVirtualList");
    IUIAutomationCondition* automationIdcondition = nullptr;
    automation->CreatePropertyCondition(UIA_AutomationIdPropertyId, automationIdProperty, &automationIdcondition);
    // collect
    std::vector<IUIAutomationCondition*> conditions;
    conditions.push_back(classNamecondition);
    conditions.push_back(automationIdcondition);
    // merge
    IUIAutomationCondition* condition = nullptr;
    if (!conditions.empty()) {
      automation->CreateAndConditionFromNativeArray(conditions.data(), static_cast<int>(conditions.size()), &condition);
    }
    // clean
    for (auto& item : conditions) {
      if (item) {
        item->Release();
      }
    }
    // ret
    return condition;
  }

  // https://msdn.microsoft.com/en-us/library/windows/desktop/ee684017(v=vs.85).aspx
  IUIAutomationCondition* BuildListItemCondition() {
    // ClassName
    VARIANT classNameProperty;
    classNameProperty.vt = VT_BSTR;
    classNameProperty.bstrVal = str2bstr("NetUIListViewItem");
    IUIAutomationCondition* classNamecondition = nullptr;
    automation->CreatePropertyCondition(UIA_ClassNamePropertyId, classNameProperty, &classNamecondition);
    // collect
    std::vector<IUIAutomationCondition*> conditions;
    conditions.push_back(classNamecondition);
    // merge
    IUIAutomationCondition* condition = nullptr;
    if (!conditions.empty()) {
      automation->CreateAndConditionFromNativeArray(conditions.data(), static_cast<int>(conditions.size()), &condition);
    }
    // clean
    for (auto& item : conditions) {
      if (item) {
        item->Release();
      }
    }
    // ret
    return condition;
  }

  // https://msdn.microsoft.com/en-us/library/windows/desktop/ee684017(v=vs.85).aspx
  IUIAutomationCondition* BuildContactNameCondition() {
    // ClassName
    VARIANT classNameProperty;
    classNameProperty.vt = VT_BSTR;
    classNameProperty.bstrVal = str2bstr("NetUISimpleButton");
    IUIAutomationCondition* classNamecondition = nullptr;
    automation->CreatePropertyCondition(UIA_ClassNamePropertyId, classNameProperty, &classNamecondition);
    // AutomationId
    VARIANT automationIdProperty;
    automationIdProperty.vt = VT_BSTR;
    automationIdProperty.bstrVal = str2bstr("idContactName");
    IUIAutomationCondition* automationIdcondition = nullptr;
    automation->CreatePropertyCondition(UIA_AutomationIdPropertyId, automationIdProperty, &automationIdcondition);
    // collect
    std::vector<IUIAutomationCondition*> conditions;
    conditions.push_back(classNamecondition);
    conditions.push_back(automationIdcondition);
    // merge
    IUIAutomationCondition* condition = nullptr;
    if (!conditions.empty()) {
      automation->CreateAndConditionFromNativeArray(conditions.data(), static_cast<int>(conditions.size()), &condition);
    }
    // clean
    for (auto& item : conditions) {
      if (item) {
        item->Release();
      }
    }
    // ret
    return condition;
  }

  void out_initContactListAutomation(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // argument 1
    auto callback = getCallbackArg(args, 1);
    // create automation
    if (!automation) {
      CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&automation));
      std::cout << "automation created" << std::endl;
    }
    HRESULT hr;
    auto inited = false;
    // get root element
    IUIAutomationElement* rootElement = nullptr;
    hr = automation->ElementFromHandle(hwnd, &rootElement);
    if (hr == S_OK && rootElement) {
      // get list view
      IUIAutomationElement* listViewElement = nullptr;
      auto listViewCondition = BuildListViewCondition();
      hr = rootElement->FindFirst(TreeScope_Descendants, listViewCondition, &listViewElement);
      listViewCondition->Release();
      if (hr == S_OK && listViewElement) {
        // remove/add event
        eventHandler = new CustomAutomationEventHandler(callback);
        automation->RemoveAutomationEventHandler(UIA_Invoke_InvokedEventId, listViewElement, eventHandler);
        automation->AddAutomationEventHandler(UIA_Invoke_InvokedEventId, listViewElement, TreeScope_Subtree, NULL, eventHandler);
        automation->RemoveAutomationEventHandler(UIA_StructureChangedEventId, listViewElement, eventHandler);
        automation->AddAutomationEventHandler(UIA_StructureChangedEventId, listViewElement, TreeScope_Subtree, NULL, eventHandler);
        setWinEventHookWrap(EVENT_OBJECT_STATECHANGE, NULL, callback); // TODO: make narrower
        inited = true;
        std::cout << "automation inited" << std::endl;
      }
    }
    // ret
    args.GetReturnValue().Set(inited);
  }

  void out_getContactListItemInfos(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    auto hwnd = getHwndArg(args, 0);
    // check
    auto infos = Nan::New<v8::Array>(0);
    if (automation) {
      HRESULT hr;
      // get root element
      IUIAutomationElement* rootElement = nullptr;
      hr = automation->ElementFromHandle(hwnd, &rootElement);
      if (hr == S_OK && rootElement) {
        // get list items
        IUIAutomationElementArray* listItems = nullptr;
        auto listItemCondition = BuildListItemCondition();
        hr = rootElement->FindAll(TreeScope_Descendants, listItemCondition, &listItems);
        listItemCondition->Release();
        if (hr == S_OK && listItems) {
          // items length
          auto length = getElementArrayLength(listItems);
          std::cout << "list items: " << length << std::endl;
          // extract infos
          auto isolate = args.GetIsolate();
          infos = Nan::New<v8::Array>(length);
          auto contactNameCondition = BuildContactNameCondition();
          for (auto index = 0; index < length; ++index) {
            auto obj = v8::Object::New(isolate);
            // get item
            IUIAutomationElement* item = nullptr;
            listItems->GetElement(index, &item);
            // find contact name
            IUIAutomationElement* contactNameElement = nullptr;
            hr = item->FindFirst(TreeScope_Descendants, contactNameCondition, &contactNameElement);
            // get item name
            if (hr == S_OK && contactNameElement) {
              BSTR bstrContactName;
              if (contactNameElement->get_CurrentName(&bstrContactName) == S_OK) {
                std::string contactName = _bstr_t(bstrContactName);
                obj->Set(Nan::New("contactName").ToLocalChecked(), Nan::New(contactName).ToLocalChecked());
              }
            }
            // get rects
            RECT rect;
            if (item->get_CurrentBoundingRectangle(&rect) == S_OK) {
              obj->Set(Nan::New("left").ToLocalChecked(), Nan::New(rect.left));
              obj->Set(Nan::New("top").ToLocalChecked(), Nan::New(rect.top));
              obj->Set(Nan::New("right").ToLocalChecked(), Nan::New(rect.right));
              obj->Set(Nan::New("bottom").ToLocalChecked(), Nan::New(rect.bottom));
            }
            // add to array
            Nan::Set(infos, index, obj);
          }
          contactNameCondition->Release();
        }
      }
    }
    // ret
    args.GetReturnValue().Set(infos);
  }

  void destroyAutomation() {
    if (automation) {
      automation->Release();
      automation = nullptr;
    }
    if (eventHandler) {
      eventHandler->Release();
      eventHandler = nullptr;
    }
  }

  /****************************************** automation end ************************************************/

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
    unhookWinEventWrap();
    destroyAutomation();
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
    exports->Set(Nan::New("setWindowRect").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_setWindowRect)->GetFunction());
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
    // automation
    exports->Set(Nan::New("initContactListAutomation").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_initContactListAutomation)->GetFunction());
    exports->Set(Nan::New("getContactListItemInfos").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_getContactListItemInfos)->GetFunction());
    // test
    exports->Set(Nan::New("helloWorld").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_helloWorld)->GetFunction());
    exports->Set(Nan::New("testCallback").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_testCallback)->GetFunction());
    // destroy
    exports->Set(Nan::New("destroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_destroy)->GetFunction());
  }

  NODE_MODULE(wb, Init);

}
