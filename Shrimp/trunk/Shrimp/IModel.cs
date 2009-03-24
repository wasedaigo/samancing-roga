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

        JToken ToJson();
        void LoadJson(JToken json);
    }

    internal class UpdatedEventArgs : EventArgs
    {
        public UpdatedEventArgs(string propertyName)
            : this(propertyName, 0, null, null)
        {
        }

        public UpdatedEventArgs(string propertyName, int itemId, object previousValue, object bounds)
        {
            this.PropertyName = propertyName;
            this.ItemId = itemId;
            this.PreviousValue = previousValue;
            this.Bounds = bounds;
        }

        public string PropertyName { get; private set; }
        public int ItemId { get; private set; }
        public object PreviousValue { get; private set; }
        public object Bounds { get; private set; }
    }
}
