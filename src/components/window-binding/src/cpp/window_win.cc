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
// #include <UIAutomation.h>
#include <UIAutomationClient.h>
#include <comdef.h>
// #include <OleAcc.h>
// #include <vector>
// #include <oleauto.h>
// #include <cassert>

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

  void out_setWindowRect(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument hwnd
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    auto hwnd = hwndMap[strHwnd];
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

  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> GetCallback(const Nan::FunctionCallbackInfo<v8::Value>& args, int index) {
    auto isolate = args.GetIsolate();
    auto arg = v8::Handle<v8::Function>::Cast(args[index]);
    v8::Persistent<v8::Function> callback(isolate, arg);
    return callback;
  }

  void out_unhookWinEvents(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    WrapUnhookWinEvent();
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectCreate(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args, 0);
    auto eventType = EVENT_OBJECT_CREATE;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectDestroy(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args, 0);
    auto eventType = EVENT_OBJECT_DESTROY;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectHide(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args, 0);
    auto eventType = EVENT_OBJECT_HIDE;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookObjectShow(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args, 0);
    auto eventType = EVENT_OBJECT_SHOW;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookLocationChange(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args, 0);
    auto eventType = EVENT_OBJECT_LOCATIONCHANGE;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookMinimizeStart(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args, 0);
    auto eventType = EVENT_SYSTEM_MINIMIZESTART;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookMinimizeEnd(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args, 0);
    auto eventType = EVENT_SYSTEM_MINIMIZEEND;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

  void out_setWinEventHookForeground(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    auto callback = GetCallback(args, 0);
    auto eventType = EVENT_SYSTEM_FOREGROUND;
    WrapSetWinEventHook(eventType, callback);
    args.GetReturnValue().Set(Nan::New(true));
  }

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
  CustomAutomationEventHandler* eventHandler;

  // https://msdn.microsoft.com/en-us/library/windows/desktop/ee684017(v=vs.85).aspx
  IUIAutomationCondition* BuildListViewCondition() {
    // ClassName
    std::string className = "NetUIVirtualListView";
    std::wstring classNameStemp = std::wstring(className.begin(), className.end());
    VARIANT classNameProperty;
    classNameProperty.vt = VT_BSTR;
    classNameProperty.bstrVal = SysAllocString(classNameStemp.c_str());
    IUIAutomationCondition* classNamecondition = nullptr;
    automation->CreatePropertyCondition(UIA_ClassNamePropertyId, classNameProperty, &classNamecondition);
    // AutomationId
    std::string automationId = "idVirtualList";
    std::wstring automationIdStemp = std::wstring(automationId.begin(), automationId.end());
    VARIANT automationIdProperty;
    automationIdProperty.vt = VT_BSTR;
    automationIdProperty.bstrVal = SysAllocString(automationIdStemp.c_str());
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
    std::string className = "NetUIListViewItem";
    std::wstring classNameStemp = std::wstring(className.begin(), className.end());
    VARIANT classNameProperty;
    classNameProperty.vt = VT_BSTR;
    classNameProperty.bstrVal = SysAllocString(classNameStemp.c_str());
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
  IUIAutomationCondition* BuildContactPhotoCondition() {
    // ClassName
    std::string className = "NetUISimpleButton";
    std::wstring classNameStemp = std::wstring(className.begin(), className.end());
    VARIANT classNameProperty;
    classNameProperty.vt = VT_BSTR;
    classNameProperty.bstrVal = SysAllocString(classNameStemp.c_str());
    IUIAutomationCondition* classNamecondition = nullptr;
    automation->CreatePropertyCondition(UIA_ClassNamePropertyId, classNameProperty, &classNamecondition);
    // AutomationId
    std::string automationId = "idContactPhoto";
    std::wstring automationIdStemp = std::wstring(automationId.begin(), automationId.end());
    VARIANT automationIdProperty;
    automationIdProperty.vt = VT_BSTR;
    automationIdProperty.bstrVal = SysAllocString(automationIdStemp.c_str());
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

  int GetElementArrayLength(IUIAutomationElementArray* elements) {
    if (elements) {
      int length = 0;
      if (SUCCEEDED(elements->get_Length(&length)))
        return length;
    }
    return 0;
  }

  std::string bstr_to_str(BSTR source){
    //source = L"lol2inside";
    _bstr_t wrapped_bstr = _bstr_t(source);
    int length = wrapped_bstr.length();
    char* char_array = new char[length];
    strcpy_s(char_array, length+1, wrapped_bstr);
    return char_array;
  }

  void out_initAutomation(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    auto hwnd = hwndMap[strHwnd];
    // argument 1
    auto callback = GetCallback(args, 1);
    // create automation
    if (!automation) {
      CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&automation));
      std::cout << "automation created" << std::endl;
    }
    // get root element
    IUIAutomationElement* rootElement = nullptr;
    automation->ElementFromHandle(hwnd, &rootElement);
    // get list view
    IUIAutomationElement* listViewElement = nullptr;
    auto listViewCondition = BuildListViewCondition();
    rootElement->FindFirst(TreeScope_Descendants, listViewCondition, &listViewElement);
    listViewCondition->Release();
    // remove and add event
    eventHandler = new CustomAutomationEventHandler(callback);
    automation->RemoveAutomationEventHandler(UIA_Invoke_InvokedEventId, listViewElement, eventHandler);
    automation->AddAutomationEventHandler(UIA_Invoke_InvokedEventId, listViewElement, TreeScope_Subtree, NULL, eventHandler);
    automation->RemoveAutomationEventHandler(UIA_StructureChangedEventId, listViewElement, eventHandler);
    automation->AddAutomationEventHandler(UIA_StructureChangedEventId, listViewElement, TreeScope_Subtree, NULL, eventHandler);
    std::cout << "automation inited" << std::endl;
  }

  void out_getContactListItemInfos(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    // argument 0
    v8::String::Utf8Value arg0(args[0]);
    auto strHwnd = std::string(*arg0);
    auto hwnd = hwndMap[strHwnd];
    // check
    if (!automation) {
      std::cout << "automation not yet inited " << std::endl;
      auto emptyArray = Nan::New<v8::Array>(0);
      args.GetReturnValue().Set(emptyArray);
      return;
    }
    // get root element
    IUIAutomationElement* rootElement = nullptr;
    automation->ElementFromHandle(hwnd, &rootElement);
    // get list items
    IUIAutomationElementArray* listItems = nullptr;
    auto listItemCondition = BuildListItemCondition();
    rootElement->FindAll(TreeScope_Descendants, listItemCondition, &listItems);
    listItemCondition->Release();
    // items length
    auto length = GetElementArrayLength(listItems);
    std::cout << "list items: " << length << std::endl;
    // extract infos
    auto isolate = args.GetIsolate();
    auto infos = Nan::New<v8::Array>(length);
    auto contactPhotoCondition = BuildContactPhotoCondition();
    for (auto index = 0; index < length; ++index) {
      auto obj = v8::Object::New(isolate);
      // get item
      IUIAutomationElement* item = nullptr;
      listItems->GetElement(index, &item);
      // find contact photo
      IUIAutomationElement* contactPhoto = nullptr;
      item->FindFirst(TreeScope_Descendants, contactPhotoCondition, &contactPhoto);
      // get item name
      std::string name;
      bool nameHasValue = false;
      if (contactPhoto) {
        BSTR bname;
        if (contactPhoto->get_CurrentName(&bname) == S_OK) {
          name = _bstr_t(bname);
          nameHasValue = true;
          obj->Set(Nan::New("name").ToLocalChecked(), Nan::New(name).ToLocalChecked());
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
    contactPhotoCondition->Release();
    // ret
    args.GetReturnValue().Set(infos);
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
    WrapUnhookWinEvent();
    hwndMap.clear();
    if (automation) {
      automation->Release();
      automation = nullptr;
    }
    if (eventHandler != NULL) {
      eventHandler->Release();
    }
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
    exports->Set(Nan::New("initAutomation").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_initAutomation)->GetFunction());
    exports->Set(Nan::New("getContactListItemInfos").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_getContactListItemInfos)->GetFunction());
    // test
    exports->Set(Nan::New("helloWorld").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_helloWorld)->GetFunction());
    exports->Set(Nan::New("testCallback").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_testCallback)->GetFunction());
    // destroy
    exports->Set(Nan::New("destroy").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(out_destroy)->GetFunction());
  }

  NODE_MODULE(wb, Init);

}
