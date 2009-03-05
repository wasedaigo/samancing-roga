using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    internal class SynchronizedTreeView : TreeView
    {
        public SynchronizedTreeView()
            : base()
        {
            this.DrawMode = TreeViewDrawMode.OwnerDrawAll;
            this.FullRowSelect = true;
            this.HideSelection = false;
            this.ItemHeight = (int)(this.ItemHeight * 1.2);
            this.Indent = this.ItemHeight;
        }

        private IEnumerable<TreeNode> AllNodes
        {
            get { return this.Traverse(this.Nodes); }
        }

        private IEnumerable<TreeNode> Traverse(TreeNodeCollection nodeCollection)
        {
            foreach (TreeNode node in nodeCollection)
            {
                yield return node;
                foreach (TreeNode node2 in this.Traverse(node.Nodes))
                {
                    yield return node2;
                }
            }
        }

        public ITree Tree
        {
            get { return this.tree; }
            set
            {
                if (this.tree != value)
                {
                    if (this.tree != null)
                    {
                        this.tree.NodeAdded -= this.Tree_NodeAdded;
                        this.tree.NodeRemoved -= this.Tree_NodeRemoved;
                    }
                    this.tree = value;
                    if (this.tree != null)
                    {
                        this.tree.NodeAdded += this.Tree_NodeAdded;
                        this.tree.NodeRemoved += this.Tree_NodeRemoved;
                        this.Initialize();
                    }
                    else
                    {
                        this.Nodes.Clear();
                    }
                }
            }
        }
        private ITree tree;

        private void Initialize()
        {
            this.Nodes.Clear();
            this.Initialize2(this.Tree.Root);
        }

        private void Initialize2(int id)
        {
            TreeNode node = new TreeNode(this.Tree.GetName(this.Tree.Root));
            node.Tag = id;
            this.Nodes.Add(node);
            foreach (int childId in this.Tree.GetChildren(id))
            {
                this.Initialize2(childId);
            }
        }

        private void Tree_NodeAdded(object sender, NodeEventArgs e)
        {
            int id = e.NodeId;
            TreeNode node = new TreeNode(this.Tree.GetName(id));
            node.Tag = id;
            int parentId = this.Tree.GetParent(id);
            TreeNode parentNode = this.AllNodes.First(n => (int)n.Tag == parentId);
            parentNode.Nodes.Add(node);
            parentNode.Expand();
            this.SelectedNode = node;
        }

        private void Tree_NodeRemoved(object sender, NodeEventArgs e)
        {
            int id = e.NodeId;
            this.AllNodes.First(n => (int)n.Tag == id).Remove();
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyUp(e);
            if (this.SelectedNode != null)
            {
                int id = (int)this.SelectedNode.Tag;
                switch (e.KeyCode)
                {
                case Keys.Insert:
                    this.Tree.Add(id);
                    break;
                case Keys.Delete:
                    if (id != this.Tree.Root)
                    {
                        this.Tree.Remove(id);
                    }
                    break;
                }
            }
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            TreeNode node = this.GetNodeAt(e.X, e.Y);
            if (node != null)
            {
                this.SelectedNode = node;
            }
        }

        protected override void OnDrawNode(DrawTreeNodeEventArgs e)
        {
            base.OnDrawNode(e);
            Graphics g = e.Graphics;
            TreeNode node = e.Node;
            Rectangle bounds = e.Bounds;
            int x = bounds.X + this.Indent * node.Level + 16;
            int y = bounds.Y + (this.ItemHeight - (int)this.Font.Size) / 2;
            string text = node.Text;
            if ((e.State & TreeNodeStates.Selected) != 0)
            {
                g.FillRectangle(SystemBrushes.Highlight, bounds);
                if ((e.State & TreeNodeStates.Focused) != 0)
                {
                    ControlPaint.DrawFocusRectangle(g, bounds,
                        SystemColors.HighlightText, SystemColors.Highlight);
                }
                g.DrawString(text, this.Font, SystemBrushes.HighlightText, x, y);
            }
            else
            {
                g.DrawString(text, this.Font, new SolidBrush(this.ForeColor), x, y);
            }
        }
    }

    internal interface ITree
    {
        int Root { get; }
        string GetName(int id);
        int GetParent(int id);
        int[] GetChildren(int id);

        void Add(int parentId);
        void Remove(int id);

        event EventHandler<NodeEventArgs> NodeAdded;
        event EventHandler<NodeEventArgs> NodeRemoved;
        event EventHandler<NodeEventArgs> NodeNameChanged;
    }

    internal class NodeEventArgs : EventArgs
    {
        public NodeEventArgs(int nodeId)
        {
            this.NodeId = nodeId;
        }

        public int NodeId { get; private set; }
    }
}
