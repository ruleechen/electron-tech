using Microsoft.Lync.Model;
using netsdk.Models;
using System;
using System.Collections.Generic;

namespace netsdk.Services
{
    public class ContactService
    {
        private LyncClientProvider lyncProvider;
        public ContactService(LyncClientProvider provider)
        {
            lyncProvider = provider;
        }

        public IEnumerable<ContactModel> SearchContacts(string searchText)
        {
            var lyncClient = lyncProvider.GetLyncClient();
            if (lyncClient == null)
            {
                throw new Exception("Can not found LyncClient.");
            }

            var result = new List<ContactModel>();

            var contactManager = lyncClient.ContactManager;
            object[] asyncState = { contactManager, searchText };

            var searchResult = contactManager.EndSearch(contactManager.BeginSearch(
                searchString: searchText,
                providers: SearchProviders.GlobalAddressList,
                searchFields: SearchFields.AllFields,
                searchOptions: SearchOptions.IncludeContactsWithoutSipOrTelUri,
                maxResults: 5,
                contactsAndGroupsCallback: null,
                state: asyncState
            ));

            foreach (var contact in searchResult.Contacts)
            {
                var model = GetContactPhoneNumbers(contact);
                result.Add(model);
            }

            return result;
        }

        private static ContactModel GetContactPhoneNumbers(Contact contact)
        {
            var contactModel = new ContactModel { Uri = contact.Uri };

            var endPoints = (List<object>)contact.GetContactInformation(ContactInformationType.ContactEndpoints);

            foreach (var item in endPoints)
            {
                var contactEndPoint = item as ContactEndpoint;
                if (contactEndPoint.Type != ContactEndpointType.Lync &&
                    contactEndPoint.Type != ContactEndpointType.Invalid &&
                    contactEndPoint.Type != ContactEndpointType.VoiceMail)
                {
                    contactModel.PhoneNumbers.Add(new ContactPhone
                    {
                        TypeName = contactEndPoint.Type.ToString(),
                        PhoneNumber = contactEndPoint.DisplayName,
                    });
                }
            }

            return contactModel;
        }
    }
}
