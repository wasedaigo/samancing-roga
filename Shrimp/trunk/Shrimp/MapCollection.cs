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
    internal class MapCollection : Model
    {
        private class Node
        {
            public Node(int id, string name)
            {
                this.Id = id;
                this.Name = name;
                this.Children = new List<Node>();
            }

            public int Id { get; private set; }
            public virtual string Name
            {
                get;
                set; // TODO
            }
            public Node Parent { get; set; }
            public List<Node> Children { get; private set; }

            public virtual JObject ToJson()
            {
                return new JObject(
                    new JProperty("Id", this.Id),
                    new JProperty("Name", this.Name),
                    new JProperty("Children",
                        new JArray(this.Children.Select(n => n.ToJson()))));
            }
        }

        private class ProjectNode : Node
        {
            public ProjectNode()
                : base(0, "Project")
            {
            }

            public override JObject ToJson()
            {
                return new JObject(
                    new JProperty("Id", this.Id),
                    new JProperty("Children",
                        new JArray(this.Children.Select(n => n.ToJson()))));
            }
        }

        private class TrashNode : Node
        {
            public TrashNode()
                : base(-1, "Trash")
            {
            }

            public override JObject ToJson()
            {
                return new JObject(
                    new JProperty("Id", this.Id),
                    new JProperty("Children",
                        new JArray(this.Children.Select(n => n.ToJson()))));
            }
        }

        public MapCollection(ViewModel viewModel)
        {
            this.ViewModel = viewModel;
            this.ProjectNodeInstance = new ProjectNode();
            this.TrashNodeInstance = new TrashNode();
            this.Clear();
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

        public int ProjectNodeId
        {
            get { return this.ProjectNodeInstance.Id; }
        }

        public int TrashNodeId
        {
            get { return this.TrashNodeInstance.Id; }
        }

        public string GetName(int id)
        {
            return this.Nodes.First(n => n.Id == id).Name;
        }

        public int GetParent(int id)
        {
            return this.Nodes.First(n => n.Id == id).Parent.Id;
        }

        public int GetRoot(int id)
        {
            Node node = this.Nodes.First(n => n.Id == id);
            while (node.Parent != null)
            {
                node = node.Parent;
            }
            return node.Id;
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
            Node node = new Node(id, "Map (ID: " + id + ")");
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

        public void Move(int id, int parentId)
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
            Node newParentNode = this.Nodes.FirstOrDefault(n => n.Id == parentId);
            if (newParentNode == null)
            {
                throw new ArgumentException("Invalid id", "parentId");
            }
            Node oldParentNode = node.Parent;
            Debug.Assert(oldParentNode != null);
            Debug.Assert(oldParentNode.Children.Contains(node));
            oldParentNode.Children.Remove(node);
            newParentNode.Children.Add(node);
            node.Parent = newParentNode;
            this.OnNodeMoved(new NodeEventArgs(id));
        }

        public override JObject ToJson()
        {
            return new JObject(
                new JProperty("Project", this.ProjectNodeInstance.ToJson()),
                new JProperty("Trash", this.TrashNodeInstance.ToJson()));
        }

        public override void LoadJson(JObject json)
        {
            this.Clear();
            foreach (JObject childJson in json["Project"]["Children"])
            {
                this.AddNodeFromJson(this.ProjectNodeInstance, childJson);
            }
            foreach (JObject childJson in json["Trash"]["Children"])
            {
                this.AddNodeFromJson(this.TrashNodeInstance, childJson);
            }
            this.OnLoaded(EventArgs.Empty);
        }

        private void AddNodeFromJson(Node parentNode, JObject json)
        {
            Node node = new Node(json.Value<int>("Id"), json.Value<string>("Name"));
            parentNode.Children.Add(node);
            node.Parent = parentNode;
            foreach (JObject childJson in json["Children"])
            {
                this.AddNodeFromJson(node, childJson);
            }
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

        public event EventHandler<NodeEventArgs> NodeMoved;
        protected virtual void OnNodeMoved(NodeEventArgs e)
        {
            if (this.NodeMoved != null) { this.NodeMoved(this, e); }
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
