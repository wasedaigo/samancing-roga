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
            this.ProjectStore =
                new SingleModelStore<Project>(new Project(), "Game.shrp");
            this.EditorStateStore =
                new SingleModelStore<EditorState>(new EditorState(this), "EditorState.json");
            this.MapCollectionStore =
                new MapCollectionStore(new MapCollection(this), "Data/MapCollection.json");
            this.TileSetCollectionStore =
                new SingleModelStore<TileSetCollection>(new TileSetCollection(this), "Data/TileSetCollection.json");

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
            get { return this.MapCollectionStore.MapCollection; }
        }
        private MapCollectionStore MapCollectionStore;

        public TileSetCollection TileSetCollection
        {
            get { return this.TileSetCollectionStore.Model; }
        }
        private SingleModelStore<TileSetCollection> TileSetCollectionStore;

        private IEnumerable<IModelStore> ModelStores
        {
            get
            {
                yield return this.ProjectStore;
                yield return this.EditorStateStore;
                yield return this.MapCollectionStore;
                yield return this.TileSetCollectionStore;
            }
        }

        public string DirectoryPath { get; private set; }

        public void New(string directoryPath, string gameTitle)
        {
            foreach (IModelStore modelStore in this.ModelStores)
            {
                modelStore.Clear();
            }
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
            foreach (IModelStore modelStore in this.ModelStores)
            {
                modelStore.Clear();
            }
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

        public Bitmap GetTilesBitmap()
        {
            string tilesBitmapPath = Path.Combine(this.DirectoryPath, "Graphics/Tiles.png");
            return Bitmap.FromFile(tilesBitmapPath) as Bitmap;
        }
    }
}

