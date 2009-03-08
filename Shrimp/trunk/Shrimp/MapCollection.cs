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
            public Node(int id, string name, bool isExpanded)
            {
                this.Id = id;
                this.Name = name;
                this.IsExpanded = isExpanded;
                this.Children = new List<Node>();
            }

            public int Id { get; private set; }
            public virtual string Name { get; private set; }
            public bool IsExpanded { get; set; }
            public Node Parent { get; set; }
            public List<Node> Children { get; private set; }

            public virtual JObject ToJson()
            {
                return new JObject(
                    new JProperty("Id", this.Id),
                    new JProperty("Name", this.Name),
                    new JProperty("IsExpanded", this.IsExpanded),
                    new JProperty("Children",
                        new JArray(this.Children.Select(n => n.ToJson()))));
            }
        }

        private abstract class RootNode : Node
        {
            public RootNode(int id, string name, bool isExpanded)
                : base(id, name, isExpanded)
            {
            }

            public override JObject ToJson()
            {
                return new JObject(
                    new JProperty("Id", this.Id),
                    new JProperty("IsExpanded", this.IsExpanded),
                    new JProperty("Children",
                        new JArray(this.Children.Select(n => n.ToJson()))));
            }
        }

        private class ProjectNode : RootNode
        {
            public ProjectNode(bool isExpanded) : base(0, "Project", isExpanded) { }
        }

        private class TrashNode : RootNode
        {
            public TrashNode(bool isExpanded) : base(-1, "Trash", isExpanded) { }
        }

        public MapCollection(ViewModel viewModel)
        {
            this.ViewModel = viewModel;
            this.ProjectNodeInstance = new ProjectNode(false);
            this.TrashNodeInstance = new TrashNode(false);
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

        private Node GetNode(int id)
        {
            Node node = this.Nodes.FirstOrDefault(n => n.Id == id);
            if (node == null)
            {
                throw new ArgumentException("Node not found", "id");
            }
            return node;
        }

        public string GetName(int id)
        {
            return this.GetNode(id).Name;
        }

        public bool IsExpanded(int id)
        {
            return this.GetNode(id).IsExpanded;
        }

        public int GetParent(int id)
        {
            return this.GetNode(id).Parent.Id;
        }

        public int GetRoot(int id)
        {
            Node node = this.GetNode(id);
            while (node.Parent != null)
            {
                node = node.Parent;
            }
            return node.Id;
        }

        public int[] GetChildren(int id)
        {
            return (from n in this.GetNode(id).Children
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
            Node node = new Node(id, "Map (ID: " + id + ")", false);
            node.Parent = this.GetNode(parentId);
            node.Parent.Children.Add(node);
            this.OnNodeAdded(new NodeEventArgs(id));
        }

        public void Remove(int id)
        {
            if (this.Roots.Contains(id))
            {
                throw new ArgumentException("Couldn't remove the root", "id");
            }
            Node node = this.GetNode(id);
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
            Node node = this.GetNode(id);
            Node newParentNode = this.GetNode(parentId);
            Node oldParentNode = node.Parent;
            Debug.Assert(oldParentNode != null);
            Debug.Assert(oldParentNode.Children.Contains(node));
            oldParentNode.Children.Remove(node);
            newParentNode.Children.Add(node);
            node.Parent = newParentNode;
            this.OnNodeMoved(new NodeEventArgs(id));
        }

        public void ExpandNode(int id)
        {
            Node node = this.GetNode(id);
            node.IsExpanded = true;
            this.OnUpdated(new UpdatedEventArgs(null));
        }

        public void CollapseNode(int id)
        {
            Node node = this.GetNode(id);
            node.IsExpanded = false;
            this.OnUpdated(new UpdatedEventArgs(null));
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
            JObject projectJson = json["Project"] as JObject;
            JObject trashJson = json["Trash"] as JObject;
            this.ProjectNodeInstance.IsExpanded = projectJson["IsExpanded"].Value<bool>();
            this.TrashNodeInstance.IsExpanded = projectJson["IsExpanded"].Value<bool>();
            foreach (JObject childJson in projectJson["Children"])
            {
                this.AddNodeFromJson(this.ProjectNodeInstance, childJson);
            }
            foreach (JObject childJson in trashJson["Children"])
            {
                this.AddNodeFromJson(this.TrashNodeInstance, childJson);
            }
            this.OnLoaded(EventArgs.Empty);
        }

        private void AddNodeFromJson(Node parentNode, JObject json)
        {
            Node node = new Node(json.Value<int>("Id"),
                json.Value<string>("Name"),
                json.Value<bool>("IsExpanded"));
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
            this.OnUpdated(new UpdatedEventArgs(null));
        }

        public event EventHandler<NodeEventArgs> NodeRemoved;
        protected virtual void OnNodeRemoved(NodeEventArgs e)
        {
            if (this.NodeRemoved != null) { this.NodeRemoved(this, e); }
            this.OnUpdated(new UpdatedEventArgs(null));
        }

        public event EventHandler<NodeEventArgs> NodeMoved;
        protected virtual void OnNodeMoved(NodeEventArgs e)
        {
            if (this.NodeMoved != null) { this.NodeMoved(this, e); }
            this.OnUpdated(new UpdatedEventArgs(null));
        }

        public event EventHandler<NodeEventArgs> NodeNameChanged;
        protected virtual void OnNodeNameChanged(NodeEventArgs e)
        {
            if (this.NodeNameChanged != null) { this.NodeNameChanged(this, e); }
            this.OnUpdated(new UpdatedEventArgs(null));
        }
    }
}
