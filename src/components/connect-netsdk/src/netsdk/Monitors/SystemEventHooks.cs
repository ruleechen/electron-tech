using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace netsdk.Monitors
{
    public class SystemEventHooks : IDisposable
    {
        public const uint EVENT_OBJECT_CREATE = 0x8000;
        public const uint EVENT_OBJECT_DESTROY = 0x8001;

        public const uint WINEVENT_OUTOFCONTEXT = 0;
        public const uint WINEVENT_SKIPOWNPROCESS = 2;

        public delegate void WinEventDelegate(
            IntPtr hWinEventHook,
            uint eventType,
            IntPtr hwnd,
            int idObject,
            int idChild,
            uint dwEventThread,
            uint dwmsEventTime
        );

        [DllImport("user32.dll")]
        public static extern IntPtr SetWinEventHook(
            uint eventMin,
            uint eventMax,
            IntPtr hmodWinEventProc,
            WinEventDelegate lpfnWinEventProc,
            uint idProcess,
            uint idThread,
            uint dwFlags
        );

        [DllImport("user32.dll")]
        public static extern bool UnhookWinEvent(IntPtr hWinEventHook);

        /*****************************************************************************/

        private LyncClientProvider lyncProvider;
        private IntPtr? objectCreatePtr;
        private IntPtr? objectDestroyPtr;

        // SetWinEventHook results in 'CallbackOnCollectedDelegate' exception
        // https://social.msdn.microsoft.com/Forums/vstudio/en-US/bddd284d-55f9-4dfd-914c-ce13d9dcc4ac/setwineventhook-results-in-callbackoncollecteddelegate-exception?forum=clr
        private WinEventDelegate objectCreateDelegate;
        private WinEventDelegate objectDestroyDelegate;

        public SystemEventHooks(LyncClientProvider provider)
        {
            lyncProvider = provider;

            objectCreateDelegate = new WinEventDelegate(OnObjectCreate);
            objectCreatePtr = SetWinEventHook(
                eventMin: EVENT_OBJECT_CREATE,
                eventMax: EVENT_OBJECT_CREATE,
                hmodWinEventProc: IntPtr.Zero,
                lpfnWinEventProc: objectCreateDelegate,
                idProcess: 0,
                idThread: 0,
                dwFlags: WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );

            objectDestroyDelegate = new WinEventDelegate(OnObjectDestroy);
            objectDestroyPtr = SetWinEventHook(
                eventMin: EVENT_OBJECT_DESTROY,
                eventMax: EVENT_OBJECT_DESTROY,
                hmodWinEventProc: IntPtr.Zero,
                lpfnWinEventProc: objectDestroyDelegate,
                idProcess: 0,
                idThread: 0,
                dwFlags: WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );
        }

        private void OnObjectCreate(IntPtr hWinEventHook, uint eventType, IntPtr hwnd, int idObject, int idChild, uint dwEventThread, uint dwmsEventTime)
        {
            RefreshLyncClient();
        }

        private void OnObjectDestroy(IntPtr hWinEventHook, uint eventType, IntPtr hwnd, int idObject, int idChild, uint dwEventThread, uint dwmsEventTime)
        {
            RefreshLyncClient();
        }

        private void RefreshLyncClient()
        {
            Task.Run(() =>
            {
                if (lyncProvider != null)
                {
                    // for sync OnChanged event
                    lyncProvider.GetLyncClient();
                }
            });
        }

        public void Dispose()
        {
            if (objectCreatePtr.HasValue)
            {
                UnhookWinEvent(objectCreatePtr.Value);
                objectCreateDelegate = null;
                objectCreatePtr = null;
            }

            if (objectDestroyPtr.HasValue)
            {
                UnhookWinEvent(objectDestroyPtr.Value);
                objectDestroyDelegate = null;
                objectDestroyPtr = null;
            }
        }
    }
}
