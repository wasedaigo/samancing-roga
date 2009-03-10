using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class ModelList<T> : Model where T : class, IModel, new()
    {
        private List<T> Models = new List<T>();

        public int Count
        {
            get
            {
                return this.Models.Count;
            }
            set
            {
                if (this.Models.Count != value)
                {
                    if (value < 0)
                    {
                        throw new ArgumentOutOfRangeException("Invalid count");
                    }
                    if (this.Models.Count < value)
                    {
                        var nulls = Enumerable.Repeat<T>(null, value - this.Models.Count);
                        this.Models.AddRange(nulls);
                    }
                    else
                    {
                        this.Models.RemoveRange(value, this.Models.Count - value);
                    }
                    Debug.Assert(this.Models.Count == value);
                    this.OnUpdated(new UpdatedEventArgs("Count"));
                }
            }
        }

        public override void Clear()
        {
            this.Models.Clear();
            this.OnCleared(EventArgs.Empty);
        }

        public override JToken ToJson()
        {
            return new JArray(this.Models.Select(m => (m != null) ? m.ToJson() : null));
        }

        public override void LoadJson(JToken json)
        {
            this.Clear();
            foreach (JObject jObject in json)
            {
                if (jObject != null)
                {
                    T model = new T();
                    model.LoadJson(jObject);
                    this.Models.Add(model);
                }
                else
                {
                    this.Models.Add(null);
                }
            }
            this.OnLoaded(EventArgs.Empty);
        }
    }
}
