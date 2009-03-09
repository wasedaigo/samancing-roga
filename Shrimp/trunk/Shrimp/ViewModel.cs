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
            this.ProjectStore = new SingleModelStore<Project>(new Project(), "Game.shrp");
            this.ModelStores.Add(this.ProjectStore);

            this.EditorStateStore = new SingleModelStore<EditorState>(new EditorState(), "EditorState.json");
            this.ModelStores.Add(this.EditorStateStore);

            this.MapCollectionStore = new MapCollectionStore(new MapCollection(this), "Data/MapCollection.json");
            this.ModelStores.Add(this.MapCollectionStore);

            foreach (IModelStore modelStore in this.ModelStores)
            {
                modelStore.IsDirtyChanged += delegate
                {
                    this.OnIsDirtyChanged(EventArgs.Empty);
                };
            }
        }

        public Project Project
        {
            get { return this.ProjectStore.Model; }
        }
        private SingleModelStore<Project> ProjectStore;

        public EditorState EditorState
        {
            get { return this.EditorStateStore.Model; }
        }
        private SingleModelStore<EditorState> EditorStateStore;

        public MapCollection MapCollection
        {
            get { return this.MapCollectionStore.Model; }
        }
        private MapCollectionStore MapCollectionStore;

        private List<IModelStore> ModelStores = new List<IModelStore>();

        private string DirectoryPath;

        public void New(string directoryPath, string gameTitle)
        {
            this.ProjectStore.Clear();
            this.MapCollectionStore.Clear();
            this.DirectoryPath = directoryPath;
            this.Project.GameTitle = gameTitle;
            Util.CopyDirectory(ProjectTemplatePath, this.DirectoryPath);
            this.Save();
            this.IsOpened = true;
        }

        private static string ProjectTemplatePath
        {
            get
            {
                string location = Assembly.GetExecutingAssembly().Location;
                return Path.Combine(Path.GetDirectoryName(location), "ProjectTemplate");
            }
        }

        public void Open(string directoryPath)
        {
            this.DirectoryPath = directoryPath;
            foreach (IModelStore modelStore in this.ModelStores)
            {
                modelStore.Load(this.DirectoryPath);
            }
            this.IsOpened = true;
        }

        public void Close()
        {
            this.ProjectStore.Clear();
            this.MapCollectionStore.Clear();
            this.IsOpened = false;
        }

        public void Save()
        {
            Debug.Assert(Directory.Exists(this.DirectoryPath));
            foreach (IModelStore modelStore in this.ModelStores)
            {
                modelStore.Save(this.DirectoryPath);
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
                return this.ModelStores.Any(m => m.IsDirty);
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

