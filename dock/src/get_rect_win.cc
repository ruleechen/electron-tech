/*
* get_rect win
*/

#include <nan.h>

namespace dock {

  void set_foreground_window() {
    HWND hwnd = FindWindow("CommunicatorMainWindowClass", NULL);
    SetForegroundWindow(hwnd);
  }

  void get_window_rect(std::string thread_name, int *left, int *top, int *right, int *bottom) {
    RECT rect = { NULL };
    HWND hwnd = FindWindow("CommunicatorMainWindowClass", NULL);
    if (GetWindowRect(hwnd, &rect)) {
      *left = rect.left;
      *top = rect.top;
      *right = rect.right;
      *bottom= rect.bottom;
    }
  }

}
