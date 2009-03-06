using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    /*internal class ProjectProxy : IProject
    {
        public ProjectProxy()
        {
            this.ModelProxy = new ModelProxy<Project>();
        }

        private Project Model
        {
            get { return this.ModelProxy.Model; }
        }
        private ModelProxy<Project> ModelProxy;

        public string GameTitle
        {
            get { return this.Model.GameTitle; }
            set { this.Model.GameTitle = value; }
        }

        public event EventHandler GameTitleChanged;

        public event EventHandler Updated;

        public JObject ToJson()
        {
            return this.Model.ToJson();
        }

        public void LoadJson(JObject json)
        {
            this.Model.LoadJson(json);
        }
    }*/
}
