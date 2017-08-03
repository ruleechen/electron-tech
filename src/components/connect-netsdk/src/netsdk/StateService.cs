using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace netsdk
{
    public class StateService
    {
        private Func<object, Task<object>> _stateChangedHandler;

        public void RegisterEvents(Func<object, Task<object>> stateChanged)
        {
            _stateChangedHandler = stateChanged;
        }
    }
}
