using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class Project : IProject
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
            this.OnUpdadted(EventArgs.Empty);
        }

        public JObject ToJson()
        {
            return new JObject(new JProperty("GameTitle", this.GameTitle));
        }

        public void LoadJson(JObject json)
        {
            this.Clear();
            this.GameTitle = json["GameTitle"].Value<string>();
        }

        public event EventHandler Updated;
        protected virtual void OnUpdadted(EventArgs e)
        {
            if (this.Updated != null) { this.Updated(this, e); }
        }
    }
}
