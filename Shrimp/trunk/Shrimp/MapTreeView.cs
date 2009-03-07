using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    internal class MapTreeView : TreeView
    {
        public MapTreeView()
            : base()
        {
            this.DrawMode = TreeViewDrawMode.OwnerDrawAll;
            this.FullRowSelect = true;
            this.HideSelection = false;
            this.ItemHeight = (int)(this.Font.Height * 1.5);
            this.Indent = this.ItemHeight;
            this.ShowLines = false;
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

        public MapCollection MapCollection
        {
            get { return this.mapCollection; }
            set
            {
                if (this.mapCollection != value)
                {
                    if (this.mapCollection != null)
                    {
                        this.mapCollection.Loaded -= this.Tree_Loaded;
                        this.mapCollection.Cleared -= this.Tree_Cleared;
                        this.mapCollection.NodeAdded -= this.Tree_NodeAdded;
                        this.mapCollection.NodeRemoved -= this.Tree_NodeRemoved;
                        this.mapCollection.NodeMoved -= this.Tree_NodeMoved;
                        this.mapCollection.NodeNameChanged -= this.Tree_NodeNameChanged;
                    }
                    this.mapCollection = value;
                    if (this.mapCollection != null)
                    {
                        this.mapCollection.Loaded += this.Tree_Loaded;
                        this.mapCollection.Cleared += this.Tree_Cleared;
                        this.mapCollection.NodeAdded += this.Tree_NodeAdded;
                        this.mapCollection.NodeRemoved += this.Tree_NodeRemoved;
                        this.mapCollection.NodeMoved += this.Tree_NodeMoved;
                        this.mapCollection.NodeNameChanged += this.Tree_NodeNameChanged;
                    }
                    this.Initialize();
                }
            }
        }
        private MapCollection mapCollection;

        private void Tree_Loaded(object sender, EventArgs e)
        {
            this.Initialize();
        }

        private void Tree_Cleared(object sender, EventArgs e)
        {
            this.Initialize();
        }

        private void Initialize()
        {
            this.Nodes.Clear();
            if (this.MapCollection != null)
            {
                foreach (int id in this.MapCollection.Roots)
                {
                    this.AddTreeNode(id);
                }
            }
        }

        private void AddTreeNode(int id)
        {
            TreeNode node = new TreeNode(this.MapCollection.GetName(id));
            node.Tag = id;
            this.Nodes.Add(node);
            foreach (int childId in this.MapCollection.GetChildren(id))
            {
                this.AddTreeNode(childId);
            }
        }

        private void Tree_NodeAdded(object sender, NodeEventArgs e)
        {
            int id = e.NodeId;
            TreeNode node = new TreeNode(this.MapCollection.GetName(id));
            node.Tag = id;
            int parentId = this.MapCollection.GetParent(id);
            TreeNode parentNode = this.AllNodes.First(n => (int)n.Tag == parentId);
            this.SuspendLayout();
            parentNode.Nodes.Add(node);
            parentNode.Expand();
            this.ResumeLayout();
            this.SelectedNode = node;
        }

        private void Tree_NodeRemoved(object sender, NodeEventArgs e)
        {
            int id = e.NodeId;
            this.SuspendLayout();
            this.AllNodes.First(n => (int)n.Tag == id).Remove();
            this.ResumeLayout();
        }

        private void Tree_NodeMoved(object sender, NodeEventArgs e)
        {
            int id = e.NodeId;
            TreeNode node = this.AllNodes.First(n => (int)n.Tag == id);
            int newParentId = this.MapCollection.GetParent(id);
            TreeNode newParentNode = this.AllNodes.First(n => (int)n.Tag == newParentId);
            this.SuspendLayout();
            node.Remove();
            newParentNode.Nodes.Add(node);
            newParentNode.Expand();
            this.ResumeLayout();
        }

        private void Tree_NodeNameChanged(object sender, NodeEventArgs e)
        {
            int id = e.NodeId;
            string text = this.MapCollection.GetName(id);
            this.SuspendLayout();
            this.AllNodes.First(n => (int)n.Tag == id).Text = text;
            this.ResumeLayout();
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyUp(e);
            if (this.SelectedNode != null)
            {
                int id = (int)this.SelectedNode.Tag;
                int rootId = this.MapCollection.GetRoot(id);
                switch (e.KeyCode)
                {
                case Keys.Insert:
                    if (rootId == this.MapCollection.ProjectNodeId)
                    {
                        this.MapCollection.Add(id);
                    }
                    break;
                case Keys.Delete:
                    if (!this.MapCollection.Roots.Contains(id))
                    {
                        if (rootId == this.MapCollection.ProjectNodeId)
                        {
                            this.MapCollection.Move(id, this.MapCollection.TrashNodeId);
                        }
                        else if (rootId == this.MapCollection.TrashNodeId)
                        {
                            DialogResult result = MessageBox.Show("Really?", "",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Information,
                                MessageBoxDefaultButton.Button2);
                            if (result == DialogResult.Yes)
                            {
                                this.MapCollection.Remove(id);
                            }
                        }
                    }
                    break;
                }
            }
        }

        protected override void OnDrawNode(DrawTreeNodeEventArgs e)
        {
            base.OnDrawNode(e);
            if (!this.Enabled) return;
            Graphics g = e.Graphics;
            TreeNode node = e.Node;
            Rectangle bounds = e.Bounds;
            int x = bounds.X + this.Indent * node.Level + 16;
            int y = bounds.Y + (this.ItemHeight - this.Font.Height) / 2;
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
}
