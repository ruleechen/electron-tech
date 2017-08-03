using netsdk.Models;
using System.Collections.Generic;
using System.Linq;

namespace netsdk
{
    public class ContactService
    {
        public IEnumerable<ContactModel> SearchContacts(string searchText)
        {
            var result = new List<ContactModel>();

            var lyncClient = Microsoft.Lync.Model.LyncClient.GetClient();
            var contactManager = lyncClient.ContactManager;

            object[] asyncState = { contactManager, searchText };

            var searchResult = contactManager.EndSearch(contactManager.BeginSearch(
                searchString: searchText,
                providers: Microsoft.Lync.Model.SearchProviders.GlobalAddressList,
                searchFields: Microsoft.Lync.Model.SearchFields.AllFields,
                searchOptions: Microsoft.Lync.Model.SearchOptions.IncludeContactsWithoutSipOrTelUri,
                maxResults: 5,
                contactsAndGroupsCallback: null,
                state: asyncState
            ));

            foreach (var contact in searchResult.Contacts)
            {
                var item = GetContactPhoneNumbers(contact);
                result.Add(item);
            }

            return result;
        }

        private static ContactModel GetContactPhoneNumbers(Microsoft.Lync.Model.Contact contact)
        {
            var contactModel = new ContactModel { Uri = contact.Uri };

            var endPoints = (List<object>)contact.GetContactInformation(Microsoft.Lync.Model.ContactInformationType.ContactEndpoints);

            foreach (var item in endPoints)
            {
                var contactEndPoint = item as Microsoft.Lync.Model.ContactEndpoint;

                if (contactEndPoint.Type != Microsoft.Lync.Model.ContactEndpointType.Lync &&
                    contactEndPoint.Type != Microsoft.Lync.Model.ContactEndpointType.Invalid &&
                    contactEndPoint.Type != Microsoft.Lync.Model.ContactEndpointType.VoiceMail)
                {
                    var type = contactEndPoint.Type.ToString();
                    contactModel.PhoneNumbers.Add(new ContactPhone
                    {
                        TypeName = type,
                        PhoneNumber = contactEndPoint.DisplayName,
                    });
                }
            }

            return contactModel;
        }
    }
}
