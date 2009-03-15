using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class TileSetCollection : Model
    {
        public TileSetCollection(ViewModel viewModel)
        {
            this.ViewModel = viewModel;
            this.Clear();
        }

        public ViewModel ViewModel { get; private set; }

        private Dictionary<int, TileSet> TileSets = new Dictionary<int, TileSet>();

        public TileSet GetItem(int id)
        {
            return this.TileSets[id];
        }

        public IEnumerable<TileSet> Items
        {
            get { return this.TileSets.Values; }
        }

        public int ItemCount
        {
            get { return this.TileSets.Count; }
        }

        public bool ContainsId(int id)
        {
            return this.TileSets.ContainsKey(id);
        }

        public IEnumerable<int> ItemIds
        {
            get { return this.TileSets.Keys; }
        }

        public int GetId(TileSet tileSet)
        {
            return this.TileSets.First(p => p.Value == tileSet).Key;
        }

        public override void Clear()
        {
            this.TileSets.Clear();
        }

        public override JToken ToJson()
        {
            return new JArray(
                from p in this.TileSets
                orderby p.Key
                select new JObject(
                    new JProperty("Id", p.Key),
                    new JProperty("Value", p.Value.ToJson())));
        }

        public override void LoadJson(JToken json)
        {
            this.Clear();
            foreach (JObject j in json as JArray)
            {
                int id = j.Value<int>("Id");
                TileSet tileSet = new TileSet(this);
                tileSet.LoadJson(j["Value"]);
                this.TileSets.Add(id, tileSet);
            }
            string path = Path.Combine(this.ViewModel.DirectoryPath, "Graphics/Tiles");
            var files = from f in Directory.GetFiles(path, "*.png", SearchOption.TopDirectoryOnly)
                        select Path.GetFileName(f);
            var registeredFiles = this.TileSets.Select(p => p.Value.ImageFileName).ToArray();
            foreach (string file in files.Except(registeredFiles))
            {
                int id = Util.GetNewId(this.TileSets.Keys);
                TileSet tileSet = new TileSet(this);
                tileSet.ImageFileName = Path.GetFileName(file);
                this.TileSets.Add(id, tileSet);
            }
        }
    }
}
