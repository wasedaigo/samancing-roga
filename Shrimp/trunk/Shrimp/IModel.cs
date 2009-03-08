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
        public UpdatedEventArgs(string propertyName)
        {
            this.PropertyName = propertyName;
        }

        public string PropertyName { get; private set; }
    }
}
