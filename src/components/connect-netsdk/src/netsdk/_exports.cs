using System.Threading.Tasks;

namespace netsdk
{
    public class exports
    {
        private ContactService _contactService;
        public ContactService ContactService
        {
            get
            {
                return _contactService ?? (_contactService = new ContactService());
            }
        }

        private ConversationService _conversationService;
        public ConversationService ConversationService
        {
            get
            {
                return _conversationService ?? (_conversationService = new ConversationService());
            }
        }

        public async Task<object> SearchContacts(dynamic args)
        {
            return _contactService.SearchContacts(args.searchText);
        }

        public async Task<object> SendMessage(dynamic args)
        {
            return _conversationService.SendMessage(args.contactUri, args.message);
        }
    }
}
