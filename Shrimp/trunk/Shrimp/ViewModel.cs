﻿using System;
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
        public ViewModel(string directoryPath)
        {
            this.DirectoryPath = directoryPath;
        }

        private string DirectoryPath { get; set; }

        public void Load()
        {
            string path = Path.Combine(this.DirectoryPath, "Game.shrp");
            Debug.Assert(File.Exists(path));
            JObject jObject = JObject.Parse(File.ReadAllText(path, new UTF8Encoding(false)));
            this.GameTitle = jObject["GameTitle"].Value<string>();
        }

        public void InitializeAndSave()
        {
            Util.CopyDirectory("ProjectTemplate", this.DirectoryPath);
            this.Save();
        }

        public void Save()
        {
            Debug.Assert(Directory.Exists(this.DirectoryPath));
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
        }

        public event EventHandler Updated;

        protected virtual void OnUpdated(EventArgs e)
        {
            if (this.Updated != null)
            {
                this.Updated(this, e);
            }
        }

        public string GameTitle
        {
            get
            {
                return this.gameTitle;
            }
            set
            {
                if (this.gameTitle != value)
                {
                    this.gameTitle = value;
                    this.OnUpdated(EventArgs.Empty);
                }
            }
        }
        private string gameTitle;

        public int TileSetIndex
        {
            get
            {
                return this.tileSetIndex;
            }
            set
            {
                if (this.tileSetIndex != value)
                {
                    this.tileSetIndex = value;
                    this.OnUpdated(EventArgs.Empty);
                }
            }
        }
        private int tileSetIndex;

        public Bitmap GetTilesBitmap()
        {
            string tilesBitmapPath =
                Path.Combine(Path.Combine(this.DirectoryPath, "Graphics"), "Tiles.png");
            return Bitmap.FromFile(tilesBitmapPath) as Bitmap;
        }
    }
}

