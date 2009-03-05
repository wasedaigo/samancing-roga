using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Text;

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
            public Node(Map map)
            {
                this.Map = map;
                this.ChildrenIds = new List<int>();
            }

            public Map Map { get; private set; }
            public virtual string Name { get { return this.Map.Name; } }
            public int ParentId { get; set; }
            public List<int> ChildrenIds { get; set; }
        }

        private class RootNode : Node
        {
            public RootNode(string name)
                : base(null)
            {
                this.name = name;
            }

            public override string Name { get { return this.name; } }
            private string name;
        }

        public MapCollection(string name)
        {
            Node node = new RootNode(name);
            this.Nodes[this.Root] = node;
        }

        private Dictionary<int, Node> Nodes = new Dictionary<int, Node>();

        public int Root
        {
            get { return 0; }
        }

        public string GetName(int id)
        {
            return this.Nodes[id].Name;
        }

        public int GetParent(int id)
        {
            return this.Nodes[id].ParentId;
        }

        public int[] GetChildren(int id)
        {
            return this.Nodes[id].ChildrenIds.ToArray();
        }

        public void Add(int parentId, string name, object tag)
        {
            if (!this.Nodes.ContainsKey(parentId))
            {
                throw new ArgumentException("Invalid id", "parentId");
            }
            int id = 0;
            int maxId = this.Nodes.Keys.Max();
            for (int i = 0; i <= maxId + 1; i++)
            {
                if (!this.Nodes.ContainsKey(i))
                {
                    id = i;
                    break;
                }
            }
            Debug.Assert(!this.Nodes.ContainsKey(id));
            Node node = new Node(new Map(name));
            node.ParentId = parentId;
            this.Nodes[id] = node;
            this.Nodes[parentId].ChildrenIds.Add(id);
            this.OnNodeAdded(new NodeEventArgs(id));
        }

        public void Remove(int id)
        {
            if (id == this.Root)
            {
                throw new ArgumentException("Couldn't remove the root", "id");
            }
            if (!this.Nodes.ContainsKey(id))
            {
                throw new ArgumentException("Invalid id", "id");
            }
            Node node = this.Nodes[id];
            Node parentNode = this.Nodes[node.ParentId];
            Debug.Assert(parentNode != null);
            Debug.Assert(parentNode.ChildrenIds.Contains(id));
            parentNode.ChildrenIds.Remove(id);
            this.Nodes.Remove(id);
            this.OnNodeRemoved(new NodeEventArgs(id));
        }

        public event EventHandler<NodeEventArgs> NodeAdded;
        protected virtual void OnNodeAdded(NodeEventArgs e)
        {
            if (this.NodeAdded != null) { this.NodeAdded(this, e); }
        }

        public event EventHandler<NodeEventArgs> NodeRemoved;
        protected virtual void OnNodeRemoved(NodeEventArgs e)
        {
            if (this.NodeRemoved != null) { this.NodeRemoved(this, e); }
        }

        public event EventHandler<NodeEventArgs> NodeNameChanged;
        protected virtual void OnNodeNameChanged(NodeEventArgs e)
        {
            if (this.NodeNameChanged != null) { this.NodeNameChanged(this, e); }
        }
    }
}
