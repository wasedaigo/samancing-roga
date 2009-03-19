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
    internal enum BitmapScale
    {
        Scale1,
        Scale2,
        Scale4,
        Scale8,
    }

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
                    this.imageFileName = value;
                    if (this.imageFileName != null)
                    {
                        this.OriginalBitmap = new Bitmap(this.ImageFileFullPath);
                    }
                    else
                    {
                        this.OriginalBitmap = null;
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
                    return this.OriginalBitmap.Width / Util.GridSize;
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
                    return this.OriginalBitmap.Height / Util.GridSize;
                }
                else
                {
                    return 0;
                }
            }
        }

        private Bitmap OriginalBitmap;

        private Dictionary<BitmapScale, Bitmap> Bitmaps =
            new Dictionary<BitmapScale, Bitmap>();

        public Bitmap GetBitmap(BitmapScale scale)
        {
            if (!this.Bitmaps.ContainsKey(scale))
            {
                switch (scale)
                {
                case BitmapScale.Scale1:
                    Bitmap bitmap = Util.CreateScaledBitmap(this.OriginalBitmap);
                    this.Bitmaps.Add(scale, bitmap);
                    return bitmap;
                default:
                    throw new NotImplementedException();
                }
            }
            else
            {
                return this.Bitmaps[scale];
            }
        }

        /*private Dictionary<BitmapScale, IntPtr> HBitmapDCs =
                    new Dictionary<BitmapScale, IntPtr>();
        private List<Graphics> graphicsCollection = new List<Graphics>(); // for GC

        public IntPtr GetHBitmapDC(BitmapScale scale)
        {
            if (!this.HBitmapDCs.ContainsKey(scale))
            {
                Bitmap bitmap = this.GetBitmap(scale);
                Graphics g = Graphics.FromImage(bitmap);
                IntPtr hBitmap = bitmap.GetHbitmap();
                IntPtr hBitmapDC = g.GetHdc();
                Win32API.SelectObject(hBitmapDC, hBitmap);
                this.graphicsCollection.Add(g);
                return this.HBitmapDCs[scale] = hBitmapDC;
            }
            else
            {
                return this.HBitmapDCs[scale];
            }
        }*/

        public override void Clear()
        {
            this.ImageFileName = null;
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
