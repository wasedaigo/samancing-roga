using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal interface IModel
    {
        void Clear();

        event EventHandler<UpdatedEventArgs> Updated;
        event EventHandler Cleared;
        event EventHandler Loaded;

        JObject ToJson();
        void LoadJson(JObject json);
    }

    internal class UpdatedEventArgs : EventArgs
    {
        public UpdatedEventArgs(string eventName, object value)
        {
            this.EventName = eventName;
            this.Value = value;
        }

        public string EventName { get; private set; }
        public object Value { get; private set; }
    }
}
