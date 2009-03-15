using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class TileSet : Model
    {
        public TileSet(TileSetCollection tileSetCollection)
        {
            this.TileSetCollection = tileSetCollection;
            this.Clear();
        }

        private TileSetCollection TileSetCollection;

        public int Id
        {
            get { return this.TileSetCollection.GetId(this); }
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
}
