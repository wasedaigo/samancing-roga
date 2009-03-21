using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class MapCollectionStore : IModelStore
    {
        public MapCollectionStore(MapCollection model, string filePath)
        {
            this.MapCollectionSingleStore =
                new SingleModelStore<MapCollection>(model, filePath);
            this.MapCollectionSingleStore.IsDirtyChanged += delegate
            {
                this.OnIsDirtyChanged(EventArgs.Empty);
            };
            this.MapCollection.NodeAdded += (s, e) =>
            {
                this.AddMap(e.NodeId);
            };
            this.MapCollection.NodeRemoved += (s, e) =>
            {
                this.RemoveMap(e.NodeId);
            };
        }

        private SingleModelStore<MapCollection> MapCollectionSingleStore;

        public string FilePath
        {
            get { return this.MapCollectionSingleStore.FilePath; }
        }

        private Dictionary<int, SingleModelStore<Map>> MapStores =
            new Dictionary<int, SingleModelStore<Map>>();

        public MapCollection MapCollection
        {
            get { return this.MapCollectionSingleStore.Model; }
        }

        private void AddMap(int id)
        {
            Map map = this.MapCollection.GetMap(id);
            string jsonFileName = "Data/Map" + id.ToString() + ".json";
            var mapStore = new SingleModelStore<Map>(map, jsonFileName);
            this.MapStores.Add(id, mapStore);
            mapStore.IsDirtyChanged += this.MapStore_IsDirtyChanged;
        }

        private void RemoveMap(int id)
        {
            var mapStore = this.MapStores[id];
            mapStore.IsDirtyChanged -= this.MapStore_IsDirtyChanged;
            this.MapStores.Remove(id);
        }

        public void Save(string directory)
        {
            this.MapCollectionSingleStore.Save(directory);
            foreach (var mapStore in this.MapStores.Values)
            {
                if (mapStore.IsDirty)
                {
                    mapStore.Save(directory);
                }
            }
        }

        public bool Load(string directory)
        {
            this.Clear();
            bool result = true;
            result &= this.MapCollectionSingleStore.Load(directory);
            foreach (int id in this.MapCollection.NodeIds)
            {
                Map map = this.MapCollection.GetMap(id);
                if (map != null)
                {
                    this.AddMap(id);
                    result &= this.MapStores[id].Load(directory);
                }
            }
            return result;
        }

        public void Clear()
        {
            this.MapCollectionSingleStore.Clear();
            foreach (var id in this.MapStores.Keys.ToArray())
            {
                this.RemoveMap(id);
            }
            this.MapStores.Clear();
        }

        private void MapStore_IsDirtyChanged(object sender, EventArgs e)
        {
            this.OnIsDirtyChanged(EventArgs.Empty);
        }

        public bool IsDirty
        {
            get
            {
                return this.MapCollectionSingleStore.IsDirty ||
                    this.MapStores.Values.Any(n => n.IsDirty);
            }
        }

        public event EventHandler IsDirtyChanged;
        protected virtual void OnIsDirtyChanged(EventArgs e)
        {
            if (this.IsDirtyChanged != null) { this.IsDirtyChanged(this, e); }
        }
    }
}
