using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class TileSet
    {
    }

    internal class TileSetCollection : Model
    {
        public TileSetCollection(ViewModel viewModel)
        {
            this.ViewModel = viewModel;
            this.ViewModel.IsOpenedChanged += delegate
            {
            };
            this.Clear();
        }

        public ViewModel ViewModel { get; private set; }

        public override void Clear()
        {
            this.OnCleared(EventArgs.Empty);
        }

        public override JToken ToJson()
        {
            return new JObject();
        }

        public override void LoadJson(JToken json)
        {
            this.OnLoaded(EventArgs.Empty);
        }
    }
}
