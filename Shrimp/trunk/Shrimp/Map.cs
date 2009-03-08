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
        public Map(string name)
        {
            this.Clear();
            this.Name = name;
        }

        public string Name
        {
            get { return this.name; }
            set
            {
                if (this.name != value)
                {
                    this.name = value;
                    this.OnUpdated(new UpdatedEventArgs("Name"));
                }
            }
        }
        private string name;

        public int Width
        {
            get { return this.width; }
            set
            {
                if (this.width != value)
                {
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
                    this.height = value;
                    this.OnUpdated(new UpdatedEventArgs("Height"));
                }
            }
        }
        private int height;

        public override void Clear()
        {
            this.Name = "";
            this.Width = 0;
            this.Height = 0;
            this.OnCleared(EventArgs.Empty);
        }

        public override JObject ToJson()
        {
            return new JObject(
                new JProperty("Name", this.Name),
                new JProperty("Width"), this.Width,
                new JProperty("Height", this.Height),
                new JProperty("Tiles", null));
        }

        public override void LoadJson(JObject json)
        {
            this.OnLoaded(EventArgs.Empty);
        }
    }
}
