using System;
using System.Collections.Generic;
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

        public MapCollection MapCollection { get; private set; }

        public int Width
        {
            get { return this.width; }
            set
            {
                if (this.width != value)
                {
                    if (value < 20)
                    {
                        throw new ArgumentOutOfRangeException("Width too small");
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
                    if (value < 15)
                    {
                        throw new ArgumentOutOfRangeException("Height too small");
                    }
                    this.height = value;
                    this.OnUpdated(new UpdatedEventArgs("Height"));
                }
            }
        }
        private int height;

        public override void Clear()
        {
            this.Width = 20;
            this.Height = 15;
            this.OnCleared(EventArgs.Empty);
        }

        public override JObject ToJson()
        {
            return new JObject(
                new JProperty("Width", this.Width),
                new JProperty("Height", this.Height),
                new JProperty("Tiles", null));
        }

        public override void LoadJson(JObject json)
        {
            this.OnLoaded(EventArgs.Empty);
        }
    }
}
