using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal interface IModelProxy
    {
        void Save(string path);
        bool Load(string path);
        void Clear();
        bool IsDirty { get; }
        event EventHandler IsDirtyChanged;
    }

    internal class ModelProxy<T> : IModelProxy where T : class, IModel
    {
        private static readonly Encoding UTF8N = new UTF8Encoding(false);

        public ModelProxy(T model)
        {
            this.Model = model;
            this.Model.Updated += delegate { this.IsDirty = true; };
            this.IsDirty = false;
        }

        public T Model { get; private set; }

        public void Save(string path)
        {
            using (var sw = new StreamWriter(path, false, UTF8N))
            using (var writer = new JsonTextWriter(sw))
            {
                writer.Formatting = Formatting.Indented;
                this.Model.ToJson().WriteTo(writer);
            }
            this.IsDirty = false;
        }

        public bool Load(string path)
        {
            this.Model.Clear();
            bool result;
            if (result = File.Exists(path))
            {
                JObject json = JObject.Parse(File.ReadAllText(path, UTF8N));
                this.Model.LoadJson(json);
            }
            this.IsDirty = false;
            return result;
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
