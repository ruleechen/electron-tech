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

        public async Task<object> SearchContacts(string searchText)
        {
            return _contactService.SearchContacts(searchText);
        }

        public async Task<object> SendMessage(string contact, string message)
        {
            return _conversationService.SendMessage(contact, message);
        }
    }
}
