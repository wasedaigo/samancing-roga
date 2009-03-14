using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class TileSet
    {
        public TileSet(string path)
        {
            this.ImagePath = path;
        }

        public string ImagePath { get; private set; }

        public string ImageFileName
        {
            get { return Path.GetFileName(this.ImagePath); }
        }
    }

    internal class TileSetCollection : Model
    {
        public TileSetCollection(ViewModel viewModel)
        {
            this.ViewModel = viewModel;
            this.ViewModel.IsOpenedChanged += delegate
            {
                this.Initialize();
            };
            this.Initialize();
        }

        public ViewModel ViewModel { get; private set; }

        private List<TileSet> TileSets = new List<TileSet>();

        public IEnumerable<TileSet> Items
        {
            get { return this.TileSets; }
        }

        private void Initialize()
        {
            if (this.ViewModel.IsOpened)
            {
                this.TileSets.Clear();
                string path = Path.Combine(this.ViewModel.DirectoryPath, "Graphics/Tiles");
                string[] files = Directory.GetFiles(path, "*.png", SearchOption.TopDirectoryOnly);
                foreach (string file in files)
                {
                    this.TileSets.Add(new TileSet(file));
                }
            }
        }

        public override void Clear()
        {
            this.TileSets.Clear();
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
