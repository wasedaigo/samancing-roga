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
        public ViewModel()
        {
            this.ProjectProxy = new ModelProxy<Project>(new Project(this));
            this.ProjectProxy.IsDirtyChanged += delegate
            {
                this.OnIsDirtyChanged(EventArgs.Empty);
            };
            this.MapCollectionProxy = new ModelProxy<MapCollection>(new MapCollection(this));
            this.MapCollectionProxy.IsDirtyChanged += delegate
            {
                this.OnIsDirtyChanged(EventArgs.Empty);
            };
        }

        public MapCollection MapCollection
        {
            get { return this.MapCollectionProxy.Model; }
        }
        private ModelProxy<MapCollection> MapCollectionProxy;

        public Project Project
        {
            get { return this.ProjectProxy.Model; }
        }
        private ModelProxy<Project> ProjectProxy;

        private string DirectoryPath;

        public void New(string directoryPath, string gameTitle)
        {
            this.ProjectProxy.Clear();
            this.MapCollectionProxy.Clear();
            this.DirectoryPath = directoryPath;
            this.Project.GameTitle = gameTitle;
            Util.CopyDirectory("ProjectTemplate", this.DirectoryPath);
            this.Save();
            this.IsOpened = true;
        }

        public void Open(string directoryPath)
        {
            this.DirectoryPath = directoryPath;
            {
                string path = Path.Combine(this.DirectoryPath, "Game.shrp");
                Debug.Assert(File.Exists(path));
                this.ProjectProxy.Load(path);
            }
            {
                string path = Path.Combine(this.DataPath, "MapCollection.json");
                this.MapCollectionProxy.Load(path);
            }
            this.IsOpened = true;
        }

        public void Close()
        {
            this.ProjectProxy.Clear();
            this.MapCollectionProxy.Clear();
            this.IsOpened = false;
        }

        private string GraphicsPath
        {
            get { return Path.Combine(this.DirectoryPath, "Graphics"); }
        }

        private string DataPath
        {
            get { return Path.Combine(this.DirectoryPath, "Data"); }
        }

        private string AudioPath
        {
            get { return Path.Combine(this.DirectoryPath, "Audio"); }
        }

        public void Save()
        {
            Debug.Assert(Directory.Exists(this.DirectoryPath));
            if (this.ProjectProxy.IsDirty)
            {
                string path = Path.Combine(this.DirectoryPath, "Game.shrp");
                this.ProjectProxy.Save(path);
            }
            if (this.MapCollectionProxy.IsDirty)
            {
                if (!Directory.Exists(this.DataPath))
                {
                    Directory.CreateDirectory(this.DataPath);
                }
                string path = Path.Combine(this.DataPath, "MapCollection.json");
                this.MapCollectionProxy.Save(path);
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
            get
            {
                return this.ProjectProxy.IsDirty || this.MapCollectionProxy.IsDirty;
            }
        }
        public event EventHandler IsDirtyChanged;
        protected virtual void OnIsDirtyChanged(EventArgs e)
        {
            if (this.IsDirtyChanged != null) { this.IsDirtyChanged(this, e); }
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

