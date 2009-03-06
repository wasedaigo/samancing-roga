using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal interface IModel
    {
        ViewModel ViewModel { get; }

        event EventHandler Updated;

        void Clear();

        JObject ToJson();

        void LoadJson(JObject json);
    }

    internal class ModelProxy<T> where T : class, IModel
    {
        public ModelProxy(T model)
        {
            this.Model = model;
            this.Model.Updated += delegate { this.IsDirty = true; };
            this.IsDirty = false;
        }

        public T Model { get; private set; }

        public void Save(string path)
        {
            using (var sw = new StreamWriter(path, false, Util.UTF8N))
            using (var writer = new JsonTextWriter(sw))
            {
                writer.Formatting = Formatting.Indented;
                this.Model.ToJson().WriteTo(writer);
            }
            this.IsDirty = false;
        }

        public void Load(string path)
        {
            this.Model.Clear();
            JObject json = JObject.Parse(File.ReadAllText(path, Util.UTF8N));
            this.Model.LoadJson(json);
            this.IsDirty = false;
        }

        public bool IsDirty
        {
            get { return this.isDirty; }
            private set
            {
                if (this.isDirty != value)
                {
                    this.isDirty = value;
                    this.OnIsDirtyChanged(EventArgs.Empty);
                }
            }
        }
        private bool isDirty = false;
        public event EventHandler IsDirtyChanged;
        protected virtual void OnIsDirtyChanged(EventArgs e)
        {
            if (this.IsDirtyChanged != null) { this.IsDirtyChanged(this, e); }
        }

        public void Clear()
        {
            this.Model.Clear();
            this.IsDirty = false;
        }
    }
}
