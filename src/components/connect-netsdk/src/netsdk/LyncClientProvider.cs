using Microsoft.Lync.Model;
using System;
using System.Threading;

namespace netsdk
{
    /// <summary>
    /// We need to use this class to get Lync client in the same thread through the application lifecycle.
    /// More info could be found here: http://stackoverflow.com/questions/9483821/lync-client-state-invalid.
    /// </summary>
    public class LyncClientProvider
    {
        private Thread _thread;
        private volatile bool _running;
        private volatile bool _clientRequested;
        private volatile bool _inSuppressedMode;
        private volatile LyncClient _lyncClient;

        private void MakeClientLoop()
        {
            while (_running)
            {
                if (_clientRequested) MakeClient();
                TryEmitChanged();
                Thread.Sleep(100);
            }
        }

        private void MakeClient()
        {
            try
            {
                _lyncClient = LyncClient.GetClient();
                // LyncClient may still exists event if its app already been closed
                // access InSuppressedMode to make sure LyncClient is available
                _inSuppressedMode = _lyncClient.InSuppressedMode;
            }
            catch (Exception)
            {
                _lyncClient = null;
            }
            _clientRequested = false;
        }

        public void Start()
        {
            if (_running) return;
            _running = true;
            _thread = new Thread(MakeClientLoop);
            _thread.Start();
        }

        public void Stop()
        {
            if (!_running) return;
            _running = false;
            _thread.Join();
            _clientRequested = false;
        }

        public LyncClient GetLyncClient()
        {
            if (!_running) return null;
            _clientRequested = true;
            while (_clientRequested) Thread.Sleep(100);
            return _lyncClient;
        }

        /**********************************************************/

        private bool previousExists = false;
        public event EventHandler<LyncClient> OnChanged;
        public void TryEmitChanged()
        {
            var exists = (_lyncClient != null);
            if (exists != previousExists && OnChanged != null)
            {
                previousExists = exists;
                OnChanged.Invoke(this, _lyncClient);
            }
        }
    }
}
