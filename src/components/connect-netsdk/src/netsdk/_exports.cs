using System.Threading.Tasks;

namespace netsdk
{
    public class exports
    {
        private ContactService _contactService;
        private ConversationService _conversationService;

        public exports()
        {
            _contactService = new ContactService();
            _conversationService = new ConversationService();
        }

        public async Task<object> SearchContacts(dynamic args)
        {
            return _contactService.SearchContacts(args.searchText);
        }

        public async Task<object> SendMessage(dynamic args)
        {
            return _conversationService.SendMessage(args.contact, args.message);
        }
    }
}
