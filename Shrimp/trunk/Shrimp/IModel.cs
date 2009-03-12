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

        JToken ToJson();
        void LoadJson(JToken json);
    }

    internal class UpdatedEventArgs : EventArgs
    {
        public UpdatedEventArgs(string propertyName)
            : this(propertyName, null)
        {
        }

        public UpdatedEventArgs(string propertyName, object tag)
        {
            this.PropertyName = propertyName;
            this.Tag = tag;
        }

        public string PropertyName { get; private set; }
        public object Tag { get; private set; }
    }
}
