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

        public override void Clear()
        {
            this.Models.Clear();
        }

        public override JToken ToJson()
        {
            return new JArray(this.Models.Select(m => m.ToJson()));
        }

        public override void LoadJson(JToken json)
        {
            this.Clear();
            foreach (JObject jObject in json)
            {
                T model = new T();
                model.LoadJson(jObject);
                this.Models.Add(model);
            }
        }
    }
}
