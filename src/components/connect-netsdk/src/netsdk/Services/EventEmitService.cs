using Microsoft.Lync.Model;
using Microsoft.Lync.Model.Conversation;
using netsdk.Models;
using System;
using System.Threading.Tasks;

namespace netsdk.Services
{
    public class EventEmitService : IDisposable
    {
        private LyncClientProvider _lyncProvider;
        private LyncClient _lastLyncClient;

        public EventEmitService(LyncClientProvider provider)
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

                try { e.ConversationManager.ConversationAdded -= ConversationAdded; } catch (Exception) { }
                try { e.ConversationManager.ConversationAdded += ConversationAdded; } catch (Exception) { }

                try { e.ConversationManager.ConversationRemoved -= ConversationRemoved; } catch (Exception) { }
                try { e.ConversationManager.ConversationRemoved += ConversationRemoved; } catch (Exception) { }

                if (_accountStateChangedHandler != null)
                {
                    _accountStateChangedHandler.Invoke(new
                    {
                        AccountState = e.State.ToString(),
                    });
                }
            }

            _lastLyncClient = e;
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

        private void ConversationAdded(object sender, ConversationManagerEventArgs e)
        {
            if (_conversationAdded != null)
            {
                _conversationAdded.Invoke(new
                {
                });
            }
        }

        private void ConversationRemoved(object sender, ConversationManagerEventArgs e)
        {
            if (_conversationRemoved != null)
            {
                _conversationRemoved.Invoke(new
                {
                });
            }
        }

        private Func<object, Task<object>> _appStateChangedHandler;
        private Func<object, Task<object>> _accountStateChangedHandler;
        private Func<object, Task<object>> _conversationAdded;
        private Func<object, Task<object>> _conversationRemoved;

        public bool RegisterEvents(
            Func<object, Task<object>> appStateChanged,
            Func<object, Task<object>> accountStateChanged,
            Func<object, Task<object>> conversationAdded,
            Func<object, Task<object>> conversationRemoved)
        {
            var isFirstAppStateHandler = (appStateChanged != null && _appStateChangedHandler == null);

            _appStateChangedHandler = appStateChanged ?? _appStateChangedHandler;
            _accountStateChangedHandler = accountStateChanged ?? _accountStateChangedHandler;
            _conversationAdded = conversationAdded ?? _conversationAdded;
            _conversationRemoved = conversationRemoved ?? _conversationRemoved;

            if (isFirstAppStateHandler)
            {
                _lyncProvider.EmitChangedEvent(force: true);
            }

            return true;
        }

        public void Dispose()
        {
            if (_lyncProvider != null)
            {
                _lyncProvider.OnChanged -= LyncProvider_OnChanged;
                _lyncProvider = null;
            }

            if (_lastLyncClient != null)
            {
                try { _lastLyncClient.StateChanged -= StateChanged; } catch (Exception) { }
                try { _lastLyncClient.ConversationManager.ConversationAdded -= ConversationAdded; } catch (Exception) { }
                try { _lastLyncClient.ConversationManager.ConversationRemoved -= ConversationRemoved; } catch (Exception) { }
                _lastLyncClient = null;
            }
        }
    }
}
