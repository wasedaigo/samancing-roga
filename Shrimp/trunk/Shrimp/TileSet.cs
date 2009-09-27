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
                    if (this.OriginalBitmap != null)
                    {
                        this.OriginalBitmap.Dispose();
                    }
                    foreach (Bitmap bitmap in this.Bitmaps.Values)
                    {
                        bitmap.Dispose();
                    }
                    this.Bitmaps.Clear();
                    this.TilePassageTypes = null;
                    this.imageFileName = value;
                    if (this.imageFileName != null)
                    {
                        this.OriginalBitmap = new Bitmap(this.ImageFileFullPath);
                        this.TilePassageTypes = new TilePassageType[this.Width * this.Height];
                    }
                    else
                    {
                        this.OriginalBitmap = null;
                    }
                    this.OnUpdated(new UpdatedEventArgs(this.GetProperty(_ => _.ImageFileName)));
                }
            }
        }
        private string imageFileName;

        public string ImageFileFullPath
        {
            get
            {
                if (this.ImageFileName != null)
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

        public int Width
        {
            get
            {
                if (this.OriginalBitmap != null)
                {
                    // TODO
                    return this.OriginalBitmap.Width / 16;
                }
                else
                {
                    return 0;
                }
            }
        }

        public int Height
        {
            get
            {
                if (this.OriginalBitmap != null)
                {
                    // TODO
                    return this.OriginalBitmap.Height / 16;
                }
                else
                {
                    return 0;
                }
            }
        }

        private Bitmap OriginalBitmap;

        private Dictionary<ScaleMode, Bitmap> Bitmaps = new Dictionary<ScaleMode, Bitmap>();

        public Bitmap GetBitmap(ScaleMode scale)
        {
            if (!this.Bitmaps.ContainsKey(scale))
            {
                Bitmap bitmap = Util.CreateScaledBitmap(this.OriginalBitmap, scale);
                this.Bitmaps.Add(scale, bitmap);
                return bitmap;
            }
            else
            {
                return this.Bitmaps[scale];
            }
        }

        public TilePassageType GetTilePassageType(int tile)
        {
            return this.TilePassageTypes[tile];
        }

        public void SetTilePassageType(int tile, TilePassageType tilePassageType)
        {
            if (this.TilePassageTypes[tile] != tilePassageType)
            {
                TilePassageType previousValue = this.TilePassageTypes[tile];
                this.TilePassageTypes[tile] = tilePassageType;
                this.OnUpdated(new UpdatedEventArgs(this.GetProperty(_ => _.TilePassageTypes), tile, previousValue, null));
            }
        }
        private TilePassageType[] TilePassageTypes;

        public override void Clear()
        {
            this.ImageFileName = null;
        }

        public override JToken ToJson()
        {
            return new JObject(
                new JProperty("ImageFileName", this.ImageFileName),
                new JProperty("TilePassageTypes", this.TilePassageTypes.Cast<int>().ToArray()));
        }

        public override void LoadJson(JToken json)
        {
            this.Clear();
            JToken token;
            if ((token = json["ImageFileName"]) != null)
            {
                this.ImageFileName = token.Value<string>();
            }
            if (this.TilePassageTypes != null)
            {
                if ((token = json["TilePassageTypes"]) != null)
                {
                    TilePassageType[] newTilePassageTypes = token.Values<int>().Cast<TilePassageType>().ToArray();
                    for (int i = 0; i < this.TilePassageTypes.Length; i++)
                    {
                        this.SetTilePassageType(i, newTilePassageTypes[i]);
                    }
                }
            }
        }
    }

    internal enum TilePassageType
    {
        Passable,
        Impassable,
        Wall,
        Ceil,
    }
}
