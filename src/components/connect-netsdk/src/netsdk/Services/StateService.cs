using Microsoft.Lync.Model;
using netsdk.Models;
using System;
using System.Threading.Tasks;

namespace netsdk.Services
{
    public class StateService
    {
        private LyncClientProvider lyncProvider;
        public StateService(LyncClientProvider provider)
        {
            lyncProvider = provider;
            lyncProvider.OnChanged += LyncProvider_OnChanged;
        }

        private void LyncProvider_OnChanged(object sender, LyncClient e)
        {
            if (e != null)
            {
                try { e.StateChanged -= StateChanged; } catch { }
                e.StateChanged += StateChanged;
            }

            if (_stateChangedHandler != null)
            {
                var connectState = (e != null) ?
                    ConnectState.Connected.ToString() :
                    ConnectState.Disconnected.ToString();

                _stateChangedHandler.Invoke(new
                {
                    Connect = connectState,
                    NewState = string.Empty,
                    OldState = string.Empty,
                    StatusCode = -1,
                });
            }
        }

        private void StateChanged(object sender, ClientStateChangedEventArgs e)
        {
            if (_stateChangedHandler != null)
            {
                _stateChangedHandler.Invoke(new
                {
                    Connect = ConnectState.Connected.ToString(),
                    NewState = e.NewState.ToString(),
                    OldState = e.OldState.ToString(),
                    StatusCode = e.StatusCode,
                });
            }
        }

        private Func<object, Task<object>> _stateChangedHandler;
        public bool RegisterEvents(Func<object, Task<object>> stateChanged)
        {
            _stateChangedHandler = stateChanged;
            return true;
        }
    }
}
