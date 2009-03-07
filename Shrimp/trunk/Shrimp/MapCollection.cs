﻿using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class MapCollection : Model, ITree
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

        private class ProjectNode : Node
        {
            public ProjectNode(MapCollection mapCollection)
                : base(0, null)
            {
                this.MapCollection = mapCollection;
            }

            public override string Name
            {
                get { return this.MapCollection.ViewModel.Project.GameTitle; }
            }
            private MapCollection MapCollection;
        }

        private class TrashNode : Node
        {
            public TrashNode()
                : base(-1, null)
            {
            }

            public override string Name { get { return "Trash"; } }
        }

        public MapCollection(ViewModel viewModel)
        {
            this.ViewModel = viewModel;
            this.ProjectNodeInstance = new ProjectNode(this);
            this.TrashNodeInstance = new TrashNode();
            this.Clear();
            this.ViewModel.Project.Updated += (s, e) =>
            {
                switch (e.EventName)
                {
                case "GameTitle":
                    this.OnNodeNameChanged(new NodeEventArgs(this.ProjectNodeInstance.Id));
                    break;
                default:
                    throw new ArgumentException("Invalid property name", "e");
                }
            };
        }

        public ViewModel ViewModel { get; private set; }

        private Node ProjectNodeInstance;
        private Node TrashNodeInstance;

        private IEnumerable<Node> Nodes
        {
            get
            {
                foreach (Node node in this.RootNodes)
                {
                    foreach (Node node2 in this.Traverse(node))
                    {
                        yield return node2;
                    }
                }
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

        public int[] Roots
        {
            get { return this.RootNodes.Select(n => n.Id).ToArray(); }
        }

        private Node[] RootNodes
        {
            get { return new[] { this.ProjectNodeInstance, this.TrashNodeInstance }; }
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
            int id = this.Roots.Min();
            int maxId = ids.Max();
            for (int i = id; i <= maxId + 1; i++)
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
            if (this.Roots.Contains(id))
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

        public override JObject ToJson()
        {
            return this.ProjectNodeInstance.ToJson();
        }

        public override void LoadJson(JObject json)
        {
            this.Clear();
            // TODO
            this.OnLoaded(EventArgs.Empty);
        }

        public override void Clear()
        {
            this.ProjectNodeInstance.Children.Clear();
            this.TrashNodeInstance.Children.Clear();
            this.OnCleared(EventArgs.Empty);
        }

        public event EventHandler<NodeEventArgs> NodeAdded;
        protected virtual void OnNodeAdded(NodeEventArgs e)
        {
            if (this.NodeAdded != null) { this.NodeAdded(this, e); }
            this.OnUpdated(new UpdatedEventArgs(null, null));
        }

        public event EventHandler<NodeEventArgs> NodeRemoved;
        protected virtual void OnNodeRemoved(NodeEventArgs e)
        {
            if (this.NodeRemoved != null) { this.NodeRemoved(this, e); }
            this.OnUpdated(new UpdatedEventArgs(null, null));
        }

        public event EventHandler<NodeEventArgs> NodeNameChanged;
        protected virtual void OnNodeNameChanged(NodeEventArgs e)
        {
            if (this.NodeNameChanged != null) { this.NodeNameChanged(this, e); }
            this.OnUpdated(new UpdatedEventArgs(null, null));
        }
    }
}
