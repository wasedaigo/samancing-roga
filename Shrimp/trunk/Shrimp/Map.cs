using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Shrimp
{
    internal class Map : IModel
    {
        public Map(string name)
        {
            this.Name = name;
        }

        public string Name { get; private set; }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public event EventHandler Updated;

        public event EventHandler Cleared;

        public event EventHandler Loaded;

        public Newtonsoft.Json.Linq.JObject ToJson()
        {
            throw new NotImplementedException();
        }

        public void LoadJson(Newtonsoft.Json.Linq.JObject json)
        {
            throw new NotImplementedException();
        }
    }
}
