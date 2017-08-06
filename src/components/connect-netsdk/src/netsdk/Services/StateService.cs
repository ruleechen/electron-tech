using Microsoft.Lync.Model;
using netsdk.Models;
using System;
using System.Threading.Tasks;

namespace netsdk.Services
{
    public class StateService
    {
        private LyncClientProvider _lyncProvider;
        public StateService(LyncClientProvider provider)
        {
            _lyncProvider = provider;
            _lyncProvider.OnChanged += LyncProvider_OnChanged;
        }

        private void LyncProvider_OnChanged(object sender, LyncClient e)
        {
            if (_appStateChangedHandler != null)
            {
                var appState = (e != null) ?
                    AppState.Available.ToString() :
                    AppState.Unavailable.ToString();

                _appStateChangedHandler.Invoke(new
                {
                    AppState = appState,
                });
            }

            if (e != null)
            {
                try { e.StateChanged -= StateChanged; } catch (Exception) { }
                try { e.StateChanged += StateChanged; } catch (Exception) { }

                if (_accountStateChangedHandler != null)
                {
                    _accountStateChangedHandler.Invoke(new
                    {
                        AccountState = e.State.ToString(),
                    });
                }
            }
        }

        private void StateChanged(object sender, ClientStateChangedEventArgs e)
        {
            if (_accountStateChangedHandler != null)
            {
                _accountStateChangedHandler.Invoke(new
                {
                    AccountState = e.NewState.ToString(),
                });
            }
        }

        private Func<object, Task<object>> _appStateChangedHandler;
        private Func<object, Task<object>> _accountStateChangedHandler;
        public bool RegisterEvents(
            Func<object, Task<object>> appStateChanged,
            Func<object, Task<object>> accountStateChanged)
        {
            _appStateChangedHandler = appStateChanged;
            _accountStateChangedHandler = accountStateChanged;
            _lyncProvider.EmitChangedEvent(force: true);
            return true;
        }
    }
}
