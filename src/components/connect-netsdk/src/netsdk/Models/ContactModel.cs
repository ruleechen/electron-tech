using System.Collections.Generic;

namespace netsdk.Models
{
    public class ContactModel
    {
        public string Uri { get; set; }

        public List<ContactPhone> PhoneNumbers { get; } = new List<ContactPhone>();
    }
}
