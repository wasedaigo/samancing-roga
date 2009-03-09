using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class ViewModel
    {
        public ViewModel()
        {
            this.ProjectProxy = new ModelProxy<Project>(new Project(), "Game.shrp");
            this.ModelProxies.Add(this.ProjectProxy);

            this.EditorStateProxy = new ModelProxy<EditorState>(new EditorState(), "EditorState.json");
            this.ModelProxies.Add(this.EditorStateProxy);

            this.MapCollectionProxy = new ModelProxy<MapCollection>(new MapCollection(this), "Data/MapCollection.json");
            this.ModelProxies.Add(this.MapCollectionProxy);

            foreach (IModelProxy modelProxy in this.ModelProxies)
            {
                modelProxy.IsDirtyChanged += delegate
                {
                    this.OnIsDirtyChanged(EventArgs.Empty);
                };
            }
        }

        public Project Project
        {
            get { return this.ProjectProxy.Model; }
        }
        private ModelProxy<Project> ProjectProxy;

        public EditorState EditorState
        {
            get { return this.EditorStateProxy.Model; }
        }
        private ModelProxy<EditorState> EditorStateProxy;

        public MapCollection MapCollection
        {
            get { return this.MapCollectionProxy.Model; }
        }
        private ModelProxy<MapCollection> MapCollectionProxy;

        private List<IModelProxy> ModelProxies = new List<IModelProxy>();

        private string DirectoryPath;

        public void New(string directoryPath, string gameTitle)
        {
            this.ProjectProxy.Clear();
            this.MapCollectionProxy.Clear();
            this.DirectoryPath = directoryPath;
            this.Project.GameTitle = gameTitle;
            string srcDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            string src = Path.Combine(srcDir, "ProjectTemplate");
            Util.CopyDirectory(src, this.DirectoryPath);
            this.Save();
            this.IsOpened = true;
        }

        public void Open(string directoryPath)
        {
            this.DirectoryPath = directoryPath;
            foreach (IModelProxy modelProxy in this.ModelProxies)
            {
                modelProxy.Load(this.DirectoryPath);
            }
            this.IsOpened = true;
        }

        public void Close()
        {
            this.ProjectProxy.Clear();
            this.MapCollectionProxy.Clear();
            this.IsOpened = false;
        }

        public void Save()
        {
            Debug.Assert(Directory.Exists(this.DirectoryPath));
            foreach (IModelProxy modelProxy in this.ModelProxies)
            {
                modelProxy.Save(this.DirectoryPath);
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
                return this.ModelProxies.Any(m => m.IsDirty);
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
            string tilesBitmapPath = Path.Combine(this.DirectoryPath, "Graphics/Tiles.png");
            return Bitmap.FromFile(tilesBitmapPath) as Bitmap;
        }
    }
}

