using netsdk.Services;
using System;
using System.Threading.Tasks;

namespace netsdk
{
    public class exports
    {
        static LyncClientProvider lyncProvider;
        static StateService stateService;
        static ContactService contactService;
        static ConversationService conversationService;

        static exports()
        {
            lyncProvider = new LyncClientProvider();
            stateService = new StateService(lyncProvider);
            contactService = new ContactService(lyncProvider);
            conversationService = new ConversationService(lyncProvider);

            lyncProvider.Start();
        }

        public async Task<object> RegisterEvents(dynamic args)
        {
            var stateChanged = (Func<object, Task<object>>)args.stateChanged;
            return stateService.RegisterEvents(
                stateChanged: stateChanged
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

            return true;
        }
    }
}
