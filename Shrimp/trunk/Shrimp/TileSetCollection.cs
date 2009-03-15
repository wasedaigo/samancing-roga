using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class TileSet : Model
    {
        public TileSet()
        {
            this.Clear();
        }

        public string ImageFileName
        {
            get { return this.imageFileName; }
            set
            {
                if (this.imageFileName != value)
                {
                    this.imageFileName = value;
                    this.OnUpdated(new UpdatedEventArgs("ImageFileName"));
                }
            }
        }
        private string imageFileName;

        public override void Clear()
        {
            this.ImageFileName = "";
        }

        public override JToken ToJson()
        {
            return new JObject(
                new JProperty("ImageFileName", this.ImageFileName));
        }

        public override void LoadJson(JToken json)
        {
            this.Clear();
            this.ImageFileName = json.Value<string>("ImageFileName");
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

        private Dictionary<int, TileSet> TileSets = new Dictionary<int, TileSet>();

        public IEnumerable<TileSet> Items
        {
            get { return (from p in this.TileSets orderby p.Key select p.Value); }
        }

        private void Initialize()
        {
            if (this.ViewModel.IsOpened)
            {
                this.TileSets.Clear();
                
            }
        }

        public override void Clear()
        {
            this.TileSets.Clear();
        }

        public override JToken ToJson()
        {
            return new JObject(
                from p in this.TileSets
                orderby p.Key
                select new JObject(
                    new JProperty("Id", p.Key),
                    new JProperty("Value", p.Value.ToJson())));
        }

        public override void LoadJson(JToken json)
        {
            this.Clear();
            foreach (JObject j in json)
            {
                int id = j.Value<int>("Id");
                TileSet tileSet = new TileSet();
                tileSet.LoadJson(json["Value"]);
                this.TileSets.Add(id, tileSet);
            }
            string path = Path.Combine(this.ViewModel.DirectoryPath, "Graphics/Tiles");
            string[] files = Directory.GetFiles(path, "*.png", SearchOption.TopDirectoryOnly);
            string[] registeredFiles = this.TileSets.Select(p => p.Value.ImageFileName).ToArray();
            foreach (string file in files.Except(registeredFiles))
            {
                int id = this.GetNewId();
                TileSet tileSet = new TileSet();
                tileSet.ImageFileName = Path.GetFileName(file);
                this.TileSets.Add(id, tileSet);
            }
        }

        private int GetNewId()
        {
            var ids = this.TileSets.Keys;
            int id = 1;
            if (0 < ids.Count)
            {
                int maxId = ids.Max();
                for (int i = id; id < maxId + 1; i++)
                {
                    if (!ids.Contains(id))
                    {
                        id = i;
                        break;
                    }
                }
            }
            return id;
        }
    }
}
