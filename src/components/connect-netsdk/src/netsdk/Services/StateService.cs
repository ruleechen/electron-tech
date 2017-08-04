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
                try { e.StateChanged -= StateChanged; } catch (Exception) { }
                try { e.StateChanged += StateChanged; } catch (Exception) { }
            }

            if (_appChangedHandler != null)
            {
                var appState = (e != null) ?
                    AppState.Connected.ToString() :
                    AppState.Disconnected.ToString();

                _appChangedHandler.Invoke(new
                {
                    AppState = appState,
                });
            }
        }

        private void StateChanged(object sender, ClientStateChangedEventArgs e)
        {
            if (_stateChangedHandler != null)
            {
                _stateChangedHandler.Invoke(new
                {
                    NewState = e.NewState.ToString(),
                    OldState = e.OldState.ToString(),
                });
            }
        }

        private Func<object, Task<object>> _appChangedHandler;
        private Func<object, Task<object>> _stateChangedHandler;
        public bool RegisterEvents(
            Func<object, Task<object>> appChanged,
            Func<object, Task<object>> stateChanged)
        {
            _appChangedHandler = appChanged;
            _stateChangedHandler = stateChanged;
            return true;
        }
    }
}
