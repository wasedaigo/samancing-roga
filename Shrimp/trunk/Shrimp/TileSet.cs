using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
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
                    if (this.imageFileName != null && this.imageFileName != "")
                    {
                        using (Image image = Image.FromFile(this.ImageFileFullPath))
                        {
                            this.Width = image.Width / Util.GridSize;
                            if (this.Width != Util.PaletteHorizontalCount)
                            {
                                throw new ArgumentException("Invalid size image");
                            }
                            this.Height = image.Height / Util.GridSize;
                        }
                    }
                    else
                    {
                        this.Width = 0;
                        this.Height = 0;
                    }
                    this.OnUpdated(new UpdatedEventArgs("ImageFileName"));
                }
            }
        }
        private string imageFileName;

        public string ImageFileFullPath
        {
            get
            {
                if (this.ImageFileName != null && this.ImageFileName != "")
                {
                    string path = this.TileSetCollection.ViewModel.DirectoryPath;
                    path = Path.Combine(path, this.TileSetCollection.TilesGraphicsDirectory);
                    path = Path.Combine(path, this.ImageFileName);
                    return path;
                }
                else
                {
                    return null;
                }
            }
        }

        public int Width { get; private set; }
        public int Height { get; private set; }

        public override void Clear()
        {
            this.ImageFileName = "";
            this.Width = 0;
            this.Height = 0;
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
