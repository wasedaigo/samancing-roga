using System;
using System.Collections.Generic;
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

        public const int MinWidth = 20;
        public const int MaxWidth = 500;
        public const int MinHeight = 15;
        public const int MaxHeight = 500;

        public MapCollection MapCollection { get; private set; }

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
                    this.width = value;
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
                    this.height = value;
                    this.OnUpdated(new UpdatedEventArgs("Height"));
                }
            }
        }
        private int height;

        public override void Clear()
        {
            this.Width = MinWidth;
            this.Height = MinHeight;
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
