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
        ViewModel ViewModel { get; }

        void Clear();

        event EventHandler Updated;
        event EventHandler Cleared;
        event EventHandler Loaded;

        JObject ToJson();
        void LoadJson(JObject json);
    }
}
