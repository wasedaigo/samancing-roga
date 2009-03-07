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

        public ITree Tree
        {
            get { return this.tree; }
            set
            {
                if (this.tree != value)
                {
                    if (this.tree != null)
                    {
                        this.tree.Loaded -= this.Tree_Loaded;
                        this.tree.Cleared -= this.Tree_Cleared;
                        this.tree.Updated -= this.Tree_Updated;
                        
                    }
                    this.tree = value;
                    if (this.tree != null)
                    {
                        this.tree.Loaded += this.Tree_Loaded;
                        this.tree.Cleared += this.Tree_Cleared;
                        this.tree.Updated += this.Tree_Updated;
                    }
                    this.Initialize();
                }
            }
        }
        private ITree tree;

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
            if (this.Tree != null)
            {
                this.AddTreeNode(this.Tree.Root);
            }
        }

        private void AddTreeNode(int id)
        {
            TreeNode node = new TreeNode(this.Tree.GetName(id));
            node.Tag = id;
            this.Nodes.Add(node);
            foreach (int childId in this.Tree.GetChildren(id))
            {
                this.AddTreeNode(childId);
            }
        }

        private void Tree_Updated(object sender, UpdatedEventArgs e)
        {
            int id = (int)e.PropertyValue;
            switch (e.PropertyName)
            {
            case "NodeAdded":
                TreeNode node = new TreeNode(this.Tree.GetName(id));
                node.Tag = id;
                int parentId = this.Tree.GetParent(id);
                TreeNode parentNode = this.AllNodes.First(n => (int)n.Tag == parentId);
                parentNode.Nodes.Add(node);
                parentNode.Expand();
                this.SelectedNode = node;
                break;
            case "NodeRemoved":
                this.AllNodes.First(n => (int)n.Tag == id).Remove();
                break;
            case "NodeNameChanged":
                string text = this.Tree.GetName(id);
                this.AllNodes.First(n => (int)n.Tag == id).Text = text;
                break;
            }
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

        protected override void OnDrawNode(DrawTreeNodeEventArgs e)
        {
            base.OnDrawNode(e);
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
