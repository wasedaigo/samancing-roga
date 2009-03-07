using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal abstract class Model : IModel
    {
        public abstract void Clear();

        public event EventHandler<UpdatedEventArgs> Updated;
        protected virtual void OnUpdated(UpdatedEventArgs e)
        {
            if (this.Updated != null) { this.Updated(this, e); }
        }

        public event EventHandler Cleared;
        protected virtual void OnCleared(EventArgs e)
        {
            if (this.Cleared != null) { this.Cleared(this, e); }
        }

        public event EventHandler Loaded;
        protected virtual void OnLoaded(EventArgs e)
        {
            if (this.Loaded != null) { this.Loaded(this, e); }
        }

        public abstract JObject ToJson();

        public abstract void LoadJson(JObject json);
    }
}
