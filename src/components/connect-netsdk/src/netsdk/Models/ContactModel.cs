using System.Collections.Generic;

namespace netsdk.Models
{
    public class ContactModel
    {
        public string Uri { get; set; }

        public Dictionary<string, string> PhoneNumbers { get; } = new Dictionary<string, string>();
    }
}
