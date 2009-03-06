using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class Project : IModel
    {
        public Project(ViewModel viewModel)
        {
            this.ViewModel = viewModel;
            this.Clear();
        }

        public ViewModel ViewModel { get; private set; }

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
            this.OnUpdadted(EventArgs.Empty);
        }

        public void Clear()
        {
            this.GameTitle = "";
            this.OnCleared(EventArgs.Empty);
        }
        public event EventHandler Cleared;
        protected virtual void OnCleared(EventArgs e)
        {
            if (this.Cleared != null) { this.Cleared(this, e); }
        }

        public JObject ToJson()
        {
            return new JObject(new JProperty("GameTitle", this.GameTitle));
        }

        public void LoadJson(JObject json)
        {
            this.Clear();
            this.GameTitle = json["GameTitle"].Value<string>();
            this.OnLoaded(EventArgs.Empty);
        }
        public event EventHandler Loaded;
        protected virtual void OnLoaded(EventArgs e)
        {
            if (this.Loaded != null) { this.Loaded(this, e); }
        }

        public event EventHandler Updated;
        protected virtual void OnUpdadted(EventArgs e)
        {
            if (this.Updated != null) { this.Updated(this, e); }
        }
    }
}
