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
            this.ProjectProxy = new ModelProxy<Project>(new Project());
            this.ModelProxies.Add(this.ProjectProxy);

            this.EditorStateProxy = new ModelProxy<EditorState>(new EditorState());
            this.ModelProxies.Add(this.EditorStateProxy);

            this.MapCollectionProxy = new ModelProxy<MapCollection>(new MapCollection(this));
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
        private string ProjectPath
        {
            get { return Path.Combine(this.DirectoryPath, "Game.shrp"); }
        }

        public EditorState EditorState
        {
            get { return this.EditorStateProxy.Model; }
        }
        private ModelProxy<EditorState> EditorStateProxy;
        private string EditorStatePath
        {
            get { return Path.Combine(this.DirectoryPath, "EditorState.json"); }
        }

        public MapCollection MapCollection
        {
            get { return this.MapCollectionProxy.Model; }
        }
        private ModelProxy<MapCollection> MapCollectionProxy;
        private string MapCollectionPath
        {
            get { return Path.Combine(this.DataPath, "MapCollection.json"); }
        }

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
            this.ProjectProxy.Load(this.ProjectPath);
            this.EditorStateProxy.Load(this.EditorStatePath);
            this.MapCollectionProxy.Load(this.MapCollectionPath);
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
                this.ProjectProxy.Save(this.ProjectPath);
            }
            if (this.EditorStateProxy.IsDirty)
            {
                this.EditorStateProxy.Save(this.EditorStatePath);
            }
            if (this.MapCollectionProxy.IsDirty)
            {
                if (!Directory.Exists(this.DataPath))
                {
                    Directory.CreateDirectory(this.DataPath);
                }
                this.MapCollectionProxy.Save(this.MapCollectionPath);
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
            string tilesBitmapPath = Path.Combine(this.GraphicsPath, "Tiles.png");
            return Bitmap.FromFile(tilesBitmapPath) as Bitmap;
        }
    }
}

