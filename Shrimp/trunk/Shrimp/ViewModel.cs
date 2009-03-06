using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class ViewModel
    {
        public MapCollection MapCollection
        {
            get { return this.mapCollection; }
            private set
            {
                if (this.mapCollection != value)
                {
                    if (this.mapCollection != null)
                    {
                        this.MapCollection.IsDirtyChanged -= this.MapCollection_IsDirtyChanged;
                    }
                    this.mapCollection = value;
                    if (this.mapCollection != null)
                    {
                        this.MapCollection.IsDirtyChanged += this.MapCollection_IsDirtyChanged;
                    }
                }
            }
        }
        private MapCollection mapCollection;
        private void MapCollection_IsDirtyChanged(object sender, EventArgs e)
        {
            this.IsDirty |= this.MapCollection.IsDirty;
        }

        private string DirectoryPath;

        public void New(string directoryPath, string gameTitle)
        {
            this.DirectoryPath = directoryPath;
            this.GameTitle = gameTitle;
            Util.CopyDirectory("ProjectTemplate", this.DirectoryPath);
            this.MapCollection = new MapCollection(this.GameTitle);
            this.Save();
            this.IsOpened = true;
            this.IsDirty = false;
        }

        public void Open(string directoryPath)
        {
            this.DirectoryPath = directoryPath;
            string path = Path.Combine(this.DirectoryPath, "Game.shrp");
            Debug.Assert(File.Exists(path));
            JObject jObject = JObject.Parse(File.ReadAllText(path, new UTF8Encoding(false)));
            this.GameTitle = jObject["GameTitle"].Value<string>();
            this.MapCollection = new MapCollection(this.GameTitle);
            this.IsOpened = true;
            this.IsDirty = false;
        }

        public void Close()
        {
            this.MapCollection = null;
            this.IsOpened = false;
            this.IsDirty = false;
        }

        private string GraphicsPath
        {
            get { return Path.Combine(this.DirectoryPath, "Graphics"); }
        }

        private string DataPath
        {
            get { return Path.Combine(this.DirectoryPath, "Data"); }
        }

        public void Save()
        {
            Debug.Assert(Directory.Exists(this.DirectoryPath));
            if (this.IsDirty)
            {
                // TODO: fix this block
                string path = Path.Combine(this.DirectoryPath, "Game.shrp");
                using (var sw = new StreamWriter(path, false, new UTF8Encoding(false)))
                using (var writer = new JsonTextWriter(sw))
                {
                    writer.Formatting = Formatting.Indented;
                    writer.WriteStartObject();
                    writer.WritePropertyName("GameTitle");
                    writer.WriteValue(this.GameTitle);
                    writer.WriteEndObject();
                }
                this.IsDirty = false;
            }
            if (this.MapCollection.IsDirty)
            {
                if (!Directory.Exists(this.DataPath))
                {
                    Directory.CreateDirectory(this.DataPath);
                }
                string path = Path.Combine(this.DataPath, "MapCollection.json");
                using (var sw = new StreamWriter(path, false, new UTF8Encoding(false)))
                using (var writer = new JsonTextWriter(sw))
                {
                    writer.Formatting = Formatting.Indented;
                    this.MapCollection.ToJson().WriteTo(writer);
                }
                this.MapCollection.IsDirty = false;
            }
        }

        public bool IsOpened
        {
            get { return this.isOpened; }
            private set
            {
                if (this.isOpened != value)
                {
                    this.isOpened = value;
                    this.OnIsOpenedChanged(EventArgs.Empty);
                }
            }
        }
        private bool isOpened = false;
        public event EventHandler IsOpenedChanged;
        protected virtual void OnIsOpenedChanged(EventArgs e)
        {
            if (this.IsOpenedChanged != null) { this.IsOpenedChanged(this, e); }
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

        public string GameTitle
        {
            get { return this.gameTitle; }
            set
            {
                if (this.gameTitle != value)
                {
                    this.gameTitle = value;
                    this.IsDirty = true;
                    this.OnGameTitleChanged(EventArgs.Empty);
                }
            }
        }
        private string gameTitle;
        public event EventHandler GameTitleChanged;
        protected virtual void OnGameTitleChanged(EventArgs e)
        {
            if (this.GameTitleChanged != null) { this.GameTitleChanged(this, e); }
        }

        public int SelectedTileSetIndex
        {
            get { return this.selectedTileSetIndex; }
            set
            {
                if (this.selectedTileSetIndex != value)
                {
                    this.selectedTileSetIndex = value;
                    this.OnSelectedTileSetIndexChanged(EventArgs.Empty);
                }
            }
        }
        private int selectedTileSetIndex;
        public event EventHandler SelectedTileSetIndexChanged;
        protected virtual void OnSelectedTileSetIndexChanged(EventArgs e)
        {
            if (this.SelectedTileSetIndexChanged != null)
            {
                this.SelectedTileSetIndexChanged(this, e);
            }
        }

        public Bitmap GetTilesBitmap()
        {
            string tilesBitmapPath = Path.Combine(this.GraphicsPath, "Tiles.png");
            return Bitmap.FromFile(tilesBitmapPath) as Bitmap;
        }
    }
}

