using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using netsdk.Models;

namespace netsdk
{
    public class ContactService
    {
        public IEnumerable<ContactModel> SearchContacts(string searchText)
        {
            return new List<ContactModel>
            {
                new ContactModel { Name = "rulee" }
            };
        }
    }
}
