/*
* get_rect win
*/

#include <nan.h>
#include <tlhelp32.h>

namespace rcsfb {

  HWND get_thread_hwnd(std::string thread_name) {
    BOOL CALLBACK enumWindowsProc(HWND hWnd, LPARAM lParam) {
      if (!IsIconic(hWnd)) {
        return TRUE;
      }
      int length = GetWindowTextLength(hWnd);
      if (length == 0) {
        return TRUE;
      }
      if (IsWindowVisible(hWnd)) {
        return FALSE;
      }
      return TRUE;
    }
    EnumWindows(enumWindowsProc, NULL);

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry) == TRUE) {
      while (Process32Next(snapshot, &entry) == TRUE) {
        if (stricmp(entry.szExeFile, thread_name.c_str()) == 0) {
          HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
          DWORD processId = GetProcessId(hProcess);
          EnumWindows(enumWindowsProc, NULL);
          CloseHandle(hProcess);
        }
      }
    }
    CloseHandle(snapshot);
    return 0;
  }

  HWND get_thread_hwnd_by_title(std::string title) {
    return FindWindow(NULL, title.c_str());
  }

  void get_window_rect(std::string thread_name, int *left, int *top, int *right, int *bottom) {
    RECT rect = { NULL };
    HWND hwnd = get_thread_hwnd(thread_name);
    if (GetWindowRect(hwnd, &rect)) {
      *left = rect.left;
      *top = rect.top;
      *right = rect.right;
      *bottom= rect.bottom;
    }
  }

}
