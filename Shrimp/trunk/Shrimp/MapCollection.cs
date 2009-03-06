using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class Map
    {
        public Map(string name)
        {
            this.Name = name;
        }

        public string Name { get; private set; }
    }

    internal class MapCollection : ITree
    {
        private class Node
        {
            public Node(int id, Map map)
            {
                this.Id = id;
                this.Map = map;
                this.Children = new List<Node>();
            }

            public int Id { get; private set; }
            public Map Map { get; private set; }
            public virtual string Name { get { return this.Map.Name; } }
            public Node Parent { get; set; }
            public List<Node> Children { get; private set; }

            public JObject ToJson()
            {
                return new JObject(
                    new JProperty("Id", this.Id),
                    new JProperty("Children",
                        new JArray(this.Children.Select(n => n.ToJson()))));
            }
        }

        private const int RootNodeId = 0;

        private class RootNode : Node
        {
            public RootNode(MapCollection mapCollection)
                : base(RootNodeId, null)
            {
                this.MapCollection = mapCollection;
            }

            public override string Name
            {
                get { return this.MapCollection.ViewModel.Project.GameTitle; }
            }
            private MapCollection MapCollection;
        }

        public MapCollection(ViewModel viewModel)
        {
            this.ViewModel = viewModel;
            this.Clear();
            this.ViewModel.Project.GameTitleChanged += delegate
            {
                this.OnNodeNameChanged(new NodeEventArgs(RootNodeId));
            };
        }

        public ViewModel ViewModel { get; private set; }

        private Node RootNodeInstance;

        private IEnumerable<Node> Nodes
        {
            get
            {
                return this.Traverse(this.RootNodeInstance);
            }
        }

        private IEnumerable<Node> Traverse(Node node)
        {
            yield return node;
            foreach (Node child in node.Children)
            {
                foreach (Node child2 in this.Traverse(child))
                {
                    yield return child2;
                }
            }
        }

        public int Root
        {
            get { return RootNodeId; }
        }

        public string GetName(int id)
        {
            return this.Nodes.First(n => n.Id == id).Name;
        }

        public int GetParent(int id)
        {
            return this.Nodes.First(n => n.Id == id).Parent.Id;
        }

        public int[] GetChildren(int id)
        {
            return (from n in this.Nodes.First(n => n.Id == id).Children
                    select n.Id).ToArray();
        }

        public void Add(int parentId)
        {
            IEnumerable<int> ids = this.Nodes.Select(n => n.Id);
            if (!ids.Contains(parentId))
            {
                throw new ArgumentException("Invalid id", "parentId");
            }
            int id = 0;
            int maxId = ids.Max();
            for (int i = 0; i <= maxId + 1; i++)
            {
                if (!ids.Contains(i))
                {
                    id = i;
                    break;
                }
            }
            Debug.Assert(!ids.Contains(id));
            Node node = new Node(id, new Map("Map (ID: " + id + ")"));
            node.Parent = this.Nodes.First(n => n.Id == parentId);
            node.Parent.Children.Add(node);
            this.OnNodeAdded(new NodeEventArgs(id));
        }

        public void Remove(int id)
        {
            if (id == this.Root)
            {
                throw new ArgumentException("Couldn't remove the root", "id");
            }
            Node node = this.Nodes.FirstOrDefault(n => n.Id == id);
            if (node == null)
            {
                throw new ArgumentException("Invalid id", "id");
            }
            Node parentNode = node.Parent;
            Debug.Assert(parentNode != null);
            Debug.Assert(parentNode.Children.Contains(node));
            parentNode.Children.Remove(node);
            this.OnNodeRemoved(new NodeEventArgs(id));
        }

        public event EventHandler<NodeEventArgs> NodeAdded;
        protected virtual void OnNodeAdded(NodeEventArgs e)
        {
            if (this.NodeAdded != null) { this.NodeAdded(this, e); }
            this.OnUpdated(EventArgs.Empty);
        }

        public event EventHandler<NodeEventArgs> NodeRemoved;
        protected virtual void OnNodeRemoved(NodeEventArgs e)
        {
            if (this.NodeRemoved != null) { this.NodeRemoved(this, e); }
            this.OnUpdated(EventArgs.Empty);
        }

        public event EventHandler<NodeEventArgs> NodeNameChanged;
        protected virtual void OnNodeNameChanged(NodeEventArgs e)
        {
            if (this.NodeNameChanged != null) { this.NodeNameChanged(this, e); }
            this.OnUpdated(EventArgs.Empty);
        }

        public JObject ToJson()
        {
            return this.RootNodeInstance.ToJson();
        }

        public void LoadJson(JObject json)
        {
            this.Clear();
            // TODO
            this.OnLoaded(EventArgs.Empty);
        }
        public event EventHandler Loaded;
        protected virtual void OnLoaded(EventArgs e)
        {
            if (this.Loaded != null) { this.Loaded(this, e); }
        }

        public void Clear()
        {
            this.RootNodeInstance = new RootNode(this);
            this.OnCleared(EventArgs.Empty);
        }
        public event EventHandler Cleared;
        protected virtual void OnCleared(EventArgs e)
        {
            if (this.Cleared != null) { this.Cleared(this, e); }
        }

        public event EventHandler Updated;
        protected virtual void OnUpdated(EventArgs e)
        {
            if (this.Updated != null) { this.Updated(this, e); }
        }
    }
}
