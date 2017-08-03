using Microsoft.Lync.Model;
using Microsoft.Lync.Model.Conversation;

namespace netsdk
{
    public class ConversationService
    {
        public bool SendMessage(string contactUri, string message)
        {
            var lyncClient = LyncClient.GetClient();
            var contactIns = lyncClient.ContactManager.GetContactByUri(contactUri);

            if (contactIns.UnifiedCommunicationType != UnifiedCommunicationType.Invalid ||
                contactIns.UnifiedCommunicationType != UnifiedCommunicationType.NotEnabled)
            {
                Conversation conversation = null;

                foreach (var con in lyncClient.ConversationManager.Conversations)
                {
                    foreach (var par in con.Participants)
                    {
                        if (par.Contact.Uri == contactIns.Uri)
                        {
                            conversation = con;
                        }
                    }
                }

                if (conversation == null)
                {
                    conversation = lyncClient.ConversationManager.AddConversation();
                    conversation.AddParticipant(contactIns);
                }

                var imModality = conversation.Modalities[ModalityTypes.InstantMessage] as InstantMessageModality;
                if (imModality.CanInvoke(ModalityAction.SendInstantMessage))
                {
                    imModality.BeginSendMessage(message, null, null);
                }
            }

            return true;
        }
    }
}