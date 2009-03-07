using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class Project : Model
    {
        public Project()
        {
            this.Clear();
        }

        public string GameTitle
        {
            get { return this.gameTitle; }
            set
            {
                if (this.gameTitle != value)
                {
                    this.gameTitle = value;
                    this.OnGameTitleChanged(EventArgs.Empty);
                }
            }
        }
        private string gameTitle;
        public event EventHandler GameTitleChanged;
        protected virtual void OnGameTitleChanged(EventArgs e)
        {
            if (this.GameTitleChanged != null) { this.GameTitleChanged(this, e); }
            this.OnUpdated(EventArgs.Empty);
        }

        public override void Clear()
        {
            this.GameTitle = "";
            this.OnCleared(EventArgs.Empty);
        }

        public override JObject ToJson()
        {
            return new JObject(new JProperty("GameTitle", this.GameTitle));
        }

        public override void LoadJson(JObject json)
        {
            this.Clear();
            this.GameTitle = json["GameTitle"].Value<string>();
            this.OnLoaded(EventArgs.Empty);
        }
    }
}
