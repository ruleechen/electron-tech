using System.Threading.Tasks;

namespace netsdk
{
    public class exports
    {
        private static StateService StateService = new StateService();
        private static ContactService ContactService = new ContactService();
        private static ConversationService ConversationService = new ConversationService();

        public async Task<object> RegisterEvents(dynamic args)
        {
            return StateService.RegisterEvents(
                stateChanged: args.stateChanged
            );
        }

        public async Task<object> SearchContacts(dynamic args)
        {
            return ContactService.SearchContacts(
                searchText: args.searchText
            );
        }

        public async Task<object> SendMessage(dynamic args)
        {
            return ConversationService.SendMessage(
                contactUri: args.contactUri,
                message: args.message
            );
        }
    }
}
