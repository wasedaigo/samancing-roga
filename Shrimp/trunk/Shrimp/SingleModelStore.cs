using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class SingleModelStore<T> : IModelStore where T : class, IModel
    {
        private static readonly Encoding UTF8N = new UTF8Encoding(false);

        public SingleModelStore(T model, string filePath)
        {
            this.Model = model;
            this.Model.Updated += delegate { this.IsDirty = true; };
            this.FilePath = filePath;
            this.IsDirty = false;
        }

        public T Model { get; private set; }

        public string FilePath { get; private set; }

        public void Save(string directory)
        {
            string path = Path.Combine(directory, this.FilePath);
            string directory2 = Path.GetDirectoryName(path);
            if (!Directory.Exists(directory2))
            {
                Directory.CreateDirectory(directory2);
            }
            using (var sw = new StreamWriter(path, false, UTF8N))
            using (var writer = new JsonTextWriter(sw))
            {
                writer.Formatting = Formatting.Indented;
                this.Model.ToJson().WriteTo(writer);
            }
            this.IsDirty = false;
        }

        public bool Load(string directory)
        {
            this.Model.Clear();
            bool result;
            string path = Path.Combine(directory, this.FilePath);
            if (result = File.Exists(path))
            {
                string jsonText = File.ReadAllText(path, UTF8N);
                JToken json;
                switch (jsonText[0])
                {
                case '{':
                    json = JObject.Parse(jsonText);
                    break;
                case '[':
                    json = JArray.Parse(jsonText);
                    break;
                default:
                    json = null;
                    break;
                }
                if (json != null)
                {
                    this.Model.LoadJson(json);
                }
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
