using System;
using System.Threading.Tasks;

namespace netsdk
{
    public class LyncProcessWatcher : IDisposable
    {
        private const string LyncProcessName = "lync.exe";
        private LyncClientProvider _lyncProvider;
        private ProcessWatcher _startupWatcher;
        private ProcessWatcher _quitWatcher;

        public LyncProcessWatcher(LyncClientProvider provider)
        {
            _lyncProvider = provider;

            _startupWatcher = ProcessWatcher.OnCreate(LyncProcessName, () =>
            {
                RefreshLyncClient($"[{LyncProcessName}] {nameof(ProcessWatcher.OnCreate)}");
            });

            _quitWatcher = ProcessWatcher.OnDelete(LyncProcessName, () =>
            {
                RefreshLyncClient($"[{LyncProcessName}] {nameof(ProcessWatcher.OnDelete)}");
            });
        }

        private void RefreshLyncClient(string eventName)
        {
            Console.WriteLine(eventName);

            Task.Run(() =>
            {
                if (_lyncProvider != null)
                {
                    // for sync OnChanged event
                    _lyncProvider.GetLyncClient();
                }
            });
        }

        public void Dispose()
        {
            if (_startupWatcher != null)
            {
                _startupWatcher.Dispose();
                _startupWatcher = null;
            }

            if (_quitWatcher != null)
            {
                _quitWatcher.Dispose();
                _quitWatcher = null;
            }
        }
    }
}
