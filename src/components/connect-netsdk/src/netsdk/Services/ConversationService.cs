using Microsoft.Lync.Model;
using Microsoft.Lync.Model.Conversation;
using System;

namespace netsdk.Services
{
    public class ConversationService
    {
        private LyncClientProvider lyncProvider;
        public ConversationService(LyncClientProvider provider)
        {
            lyncProvider = provider;
        }

        public bool SendMessage(string contactUri, string message)
        {
            Console.WriteLine($"Start send message to <{contactUri}> with message <{message}>");

            var lyncClient = lyncProvider.GetLyncClient();
            if (lyncClient == null)
            {
                throw new Exception("Can not found LyncClient.");
            }

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

            Console.WriteLine("End send message");

            return true;
        }
    }
}