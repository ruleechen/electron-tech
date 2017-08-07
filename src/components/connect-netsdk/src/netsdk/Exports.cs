using netsdk.Monitors;
using netsdk.Services;
using System;
using System.Threading.Tasks;

namespace netsdk
{
    public class exports
    {
        static LyncClientProvider lyncProvider;
        static LyncProcessWatcher lyncWatcher;
        static SystemEventHooks systemEvents;
        static EventEmitService eventService;
        static ContactService contactService;
        static ConversationService conversationService;

        static exports()
        {
            lyncProvider = new LyncClientProvider();
            lyncWatcher = new LyncProcessWatcher(lyncProvider);
            systemEvents = new SystemEventHooks(lyncProvider);
            eventService = new EventEmitService(lyncProvider);
            contactService = new ContactService(lyncProvider);
            conversationService = new ConversationService(lyncProvider);

            lyncProvider.Start();
        }

        public async Task<object> LaunchLync(dynamic args)
        {
            LyncLauncher.StartClient();
            return true;
        }

        public async Task<object> RegisterEvents(dynamic args)
        {
            var appStateChanged = (Func<object, Task<object>>)args.appStateChanged;
            var accountStateChanged = (Func<object, Task<object>>)args.accountStateChanged;
            return eventService.RegisterEvents(
                appStateChanged: appStateChanged,
                accountStateChanged: accountStateChanged
            );
        }

        public async Task<object> SearchContacts(dynamic args)
        {
            var searchText = (string)args.searchText;
            return contactService.SearchContacts(
                searchText: searchText
            );
        }

        public async Task<object> SendMessage(dynamic args)
        {
            var contactUri = (string)args.contactUri;
            var message = (string)args.message;
            return conversationService.SendMessage(
                contactUri: contactUri,
                message: message
            );
        }

        public async Task<object> Destroy(dynamic args)
        {
            if (lyncProvider != null)
            {
                lyncProvider.Stop();
                lyncProvider = null;
            }

            if (lyncWatcher != null)
            {
                lyncWatcher.Dispose();
                lyncWatcher = null;
            }

            if (systemEvents != null)
            {
                systemEvents.Dispose();
                systemEvents = null;
            }

            if (eventService != null)
            {
                eventService.Dispose();
                eventService = null;
            }

            Console.WriteLine("netsdk destroy");

            return true;
        }
    }
}
