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
            this.InitializeComponent();
            this.DrawMode = TreeViewDrawMode.OwnerDrawAll;
            this.FullRowSelect = true;
            this.HideSelection = false;
            this.ItemHeight = (int)(this.Font.Height * 1.8);
            this.Indent = 19;
            this.ShowLines = false;
            this.ShowRootLines = false;
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

        private ImageList imageList;

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
            this.AllNodes.First(n => (int)n.Tag == this.MapCollection.ProjectNodeId).ImageKey = "Folder";
            this.AllNodes.First(n => (int)n.Tag == this.MapCollection.TrashNodeId).ImageKey = "Bin";
        }

        private void AddTreeNode(int id)
        {
            TreeNode node = new TreeNode(this.MapCollection.GetName(id));
            node.ImageKey = "PageWhite";
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
            node.ImageKey = "PageWhite";
            node.Tag = id;
            int parentId = this.MapCollection.GetParent(id);
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

        private void Tree_NodeMoved(object sender, NodeEventArgs e)
        {
            int id = e.NodeId;
            TreeNode node = this.AllNodes.First(n => (int)n.Tag == id);
            int newParentId = this.MapCollection.GetParent(id);
            TreeNode newParentNode = this.AllNodes.First(n => (int)n.Tag == newParentId);
            node.Remove();
            newParentNode.Nodes.Add(node);
            newParentNode.Expand();
            this.Invalidate();
        }

        private void Tree_NodeNameChanged(object sender, NodeEventArgs e)
        {
            int id = e.NodeId;
            string text = this.MapCollection.GetName(id);
            this.AllNodes.First(n => (int)n.Tag == id).Text = text;
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
            bool isSelected = (e.State & TreeNodeStates.Selected) != 0;
            if (isSelected)
            {
                g.FillRectangle(SystemBrushes.Highlight, bounds);
                if ((e.State & TreeNodeStates.Focused) != 0)
                {
                    ControlPaint.DrawFocusRectangle(g, bounds,
                        SystemColors.HighlightText, SystemColors.Highlight);
                }
            }
            if (0 < node.Level && 0 < node.GetNodeCount(false))
            {
                Image toggleImage = 
                    this.ImageList.Images["BulletToggle" + (node.IsExpanded ? "Minus" : "Plus")];
                g.DrawImage(toggleImage, bounds.X + this.Indent * (node.Level - 1) + 1, bounds.Y + 1);
            }
            Image image = this.ImageList.Images[node.ImageKey];
            if (image != null)
            {
                g.DrawImage(image, bounds.X + this.Indent * node.Level + 1, bounds.Y + 1);
            }
            g.DrawString(node.Text, this.Font,
                isSelected ? SystemBrushes.HighlightText : new SolidBrush(this.ForeColor),
                bounds.X + this.Indent * node.Level + 16,
                bounds.Y + (this.ItemHeight - this.Font.Height) / 2);
        }

        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MapTreeView));
            this.imageList = new System.Windows.Forms.ImageList(this.components);
            this.SuspendLayout();
            // 
            // imageList
            // 
            this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
            this.imageList.TransparentColor = System.Drawing.Color.Empty;
            this.imageList.Images.SetKeyName(0, "PageWhite");
            this.imageList.Images.SetKeyName(1, "Bin");
            this.imageList.Images.SetKeyName(2, "Folder");
            this.imageList.Images.SetKeyName(3, "BulletTogglePlus");
            this.imageList.Images.SetKeyName(4, "BulletToggleMinus");
            // 
            // MapTreeView
            // 
            this.ImageKey = "PageWhite";
            this.ImageList = this.imageList;
            this.SelectedImageIndex = 0;
            this.ResumeLayout(false);

        }

        private System.ComponentModel.IContainer components;
    }
}
