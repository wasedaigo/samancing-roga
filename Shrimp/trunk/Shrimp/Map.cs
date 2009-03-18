using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class Map : Model
    {
        public Map(MapCollection mapCollection)
        {
            this.MapCollection = mapCollection;
            this.Clear();
        }

        private const int LayerCount = 2;

        public const int MinWidth = 20;
        public const int MaxWidth = 500;
        public const int MinHeight = 15;
        public const int MaxHeight = 500;

        public MapCollection MapCollection { get; private set; }

        public int Id
        {
            get { return this.MapCollection.GetId(this); }
        }

        public string Name
        {
            get { return this.MapCollection.GetName(this.Id); }
            set { this.MapCollection.SetName(this.Id, value); }
        }

        public int Width
        {
            get { return this.width; }
            set
            {
                if (this.width != value)
                {
                    if (value < MinWidth || MaxWidth < value)
                    {
                        throw new ArgumentOutOfRangeException("Invalid width");
                    }
                    if (this.width < value)
                    {
                        foreach (var layer in this.Layers)
                        {
                            for (int j = 0; j < this.height; j++)
                            {
                                var newTiles = Enumerable.Repeat(new Tile(), value - this.width);
                                layer.InsertRange(j * value + this.width, newTiles);
                            }
                        }
                    }
                    else
                    {
                        foreach (var layer in this.Layers)
                        {
                            for (int j = 0; j < this.height; j++)
                            {
                                layer.RemoveRange(j * value + value, this.width - value);
                            }
                        }
                    }
                    this.width = value;
                    Debug.Assert(this.Layers.All(l => l.Count == this.Width * this.Height));
                    this.OnUpdated(new UpdatedEventArgs("Width"));
                }
            }
        }
        private int width;

        public int Height
        {
            get { return this.height; }
            set
            {
                if (this.height != value)
                {
                    if (value < MinHeight || MaxHeight < value)
                    {
                        throw new ArgumentOutOfRangeException("Invalid height");
                    }
                    if (this.height < value)
                    {
                        foreach (var layer in this.Layers)
                        {
                            int size = (value - this.height) * this.Width;
                            layer.AddRange(Enumerable.Repeat(new Tile(), size));
                        }
                    }
                    else
                    {
                        foreach (var layer in this.Layers)
                        {
                            int size = (this.height - value) * this.Width;
                            layer.RemoveRange(layer.Count - size, size);
                        }
                    }
                    this.height = value;
                    Debug.Assert(this.Layers.All(l => l.Count == this.Width * this.Height));
                    this.OnUpdated(new UpdatedEventArgs("Height"));
                }
            }
        }
        private int height;

        private List<List<Tile>> Layers = new List<List<Tile>>();

        public Tile GetTile(int layerNumber, int x, int y)
        {
            return this.Layers[layerNumber][y * this.Width + x];
        }

        public void SetTile(int layerNumber, int x, int y, Tile tile)
        {
            this.Layers[layerNumber][y * this.Width + x] = tile;
            this.OnUpdated(new UpdatedEventArgs("Tiles"));
        }

        public void SetTiles(int layerNumber, int x, int y, SelectedTiles selectedTiles)
        {
            var tiles = selectedTiles.Tiles.ToArray();
            List<Tile> layer = this.Layers[layerNumber];
            for (int j = 0; j < selectedTiles.Height; j++)
            {
                for (int i = 0; i < selectedTiles.Width; i++)
                {
                    Tile tile = tiles[j * selectedTiles.Width + i];
                    if (0 <= i + x && i + x < this.Width &&
                        0 <= j + y && j + y < this.Height)
                    {
                        layer[(j + y) * this.Width + (i + x)] = tile;
                    }
                }
            }
            this.OnUpdated(new UpdatedEventArgs("Tiles"));
        }

        public override void Clear()
        {
            this.Width = MinWidth;
            this.Height = MinHeight;
            this.Layers.Clear();
            int size = this.Width * this.Height;
            for (int i = 0; i < LayerCount; i++)
            {
                this.Layers.Add(Enumerable.Repeat(new Tile(), size).ToList());
            }
        }

        public override JToken ToJson()
        {
            return new JObject(
                new JProperty("Width", this.Width),
                new JProperty("Height", this.Height),
                new JProperty("Tiles", null));
        }

        public override void LoadJson(JToken json)
        {
            this.Width = json["Width"].Value<int>();
            this.Height = json["Height"].Value<int>();
        }
    }
}
