using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Win32Hook
{
    public class Startup
    {
        static Startup()
        {
            var process = System.Diagnostics.Process.GetProcessesByName("lync").FirstOrDefault();
            var dele = new Win32.WinEventDelegate(WinEventProc);
            // Win32.SetWinEventHook(Win32.EVENT_OBJECT_LOCATIONCHANGE, Win32.EVENT_OBJECT_LOCATIONCHANGE, process.MainWindowHandle, dele, 0, 0, Win32.WINEVENT_OUTOFCONTEXT);
        }

        private static void WinEventProc(IntPtr hWinEventHook, uint eventType, IntPtr hwnd, int idObject, int idChild, uint dwEventThread, uint dwmsEventTime)
        {
            Win32.Rect rect = new Win32.Rect();
            Win32.GetWindowRect(hWinEventHook, ref rect);
            System.IO.File.AppendAllText("rect.log", rect.Top.ToString() + "\r\n");
        }

        public async Task<object> Invoke(object input)
        {
            var process = System.Diagnostics.Process.GetProcessesByName("lync").FirstOrDefault();

            Win32.BringWindowToFront(process.MainWindowHandle);

            Win32.Rect rect = new Win32.Rect();
            Win32.GetWindowRect(process.MainWindowHandle, ref rect);

            Win32.SetWindowPos(process.MainWindowHandle, 0, rect.Left + 10, rect.Top + 10, 600, 800, 0x0004);

            return new
            {
                processId = process.Id,
                visible = Win32.IsWindowVisible(process.Id),
                rect = rect
            };
        }
    }
}
