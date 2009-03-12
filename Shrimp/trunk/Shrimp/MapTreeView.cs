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
            this.contextMenuStrip.Opening += (s, e) =>
            {
                Point location = new Point
                {
                    X = this.contextMenuStrip.Left,
                    Y = this.contextMenuStrip.Top,
                };
                this.SelectedNode = this.GetNodeAt(this.PointToClient(location));
                if (this.SelectedNode != null)
                {
                    int id = (int)this.SelectedNode.Tag;
                    int rootId = this.MapCollection.GetRoot(id);
                    this.EditToolStripMenuItem.Enabled =
                        !this.MapCollection.Roots.Contains(id);
                    this.InsertToolStripMenuItem.Enabled =
                        (rootId == this.MapCollection.ProjectNodeId);
                    this.DeleteToolStripMenuItem.Enabled =
                        !this.MapCollection.Roots.Contains(id);
                }
            };
            this.EditToolStripMenuItem.Click += (s, e) =>
            {
                if (this.SelectedNode != null)
                {
                    int id = (int)this.SelectedNode.Tag;
                    string name = this.MapCollection.GetName(id);
                    Map map = this.MapCollection.GetMap(id);
                    using (var dialog = new MapDialog(id, name, map))
                    {
                        if (dialog.ShowDialog() == DialogResult.OK)
                        {
                            this.MapCollection.SetName(id, dialog.MapName);
                            map.Width = dialog.MapWidth;
                            map.Height = dialog.MapHeight;
                        }
                    }
                }
            };
            this.InsertToolStripMenuItem.Click += (s, e) =>
            {
                if (this.SelectedNode != null)
                {
                    int selectedNodeId = (int)this.SelectedNode.Tag;
                    Debug.Assert(this.MapCollection.GetRoot(selectedNodeId) ==
                        this.MapCollection.ProjectNodeId);
                    int newId = this.MapCollection.GetNewId();
                    using (var dialog = new MapDialog(newId, "", null))
                    {
                        if (dialog.ShowDialog() == DialogResult.OK)
                        {
                            newId = this.MapCollection.Add(selectedNodeId, dialog.MapName);
                            Map map = this.MapCollection.GetMap(newId);
                            map.Width = dialog.MapWidth;
                            map.Height = dialog.MapHeight;
                        }
                    }
                }
            };
            this.DeleteToolStripMenuItem.Click += (s, e) =>
            {
                if (this.SelectedNode != null)
                {
                    int selectedNodeId = (int)this.SelectedNode.Tag;
                    int rootId = this.MapCollection.GetRoot(selectedNodeId);
                    if (!this.MapCollection.Roots.Contains(selectedNodeId))
                    {
                        if (rootId == this.MapCollection.ProjectNodeId)
                        {
                            this.MapCollection.Move(selectedNodeId, this.MapCollection.TrashNodeId);
                        }
                        else if (rootId == this.MapCollection.TrashNodeId)
                        {
                            DialogResult result = MessageBox.Show("Really?", "",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Question,
                                MessageBoxDefaultButton.Button2);
                            if (result == DialogResult.Yes)
                            {
                                this.MapCollection.Remove(selectedNodeId);
                            }
                        }
                    }
                }
            };

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
        private ContextMenuStrip contextMenuStrip;
        private ToolStripMenuItem InsertToolStripMenuItem;
        private ToolStripMenuItem DeleteToolStripMenuItem;
        private ToolStripMenuItem EditToolStripMenuItem;
        private ToolStripSeparator toolStripSeparator1;

        public ViewModel ViewModel
        {
            get { return this.viewModel; }
            set
            {
                if (this.viewModel != value)
                {
                    if (this.viewModel != null)
                    {
                        this.viewModel.MapCollection.Loaded -= this.Tree_Loaded;
                        this.viewModel.MapCollection.Cleared -= this.Tree_Cleared;
                        this.viewModel.MapCollection.NodeAdded -= this.Tree_NodeAdded;
                        this.viewModel.MapCollection.NodeRemoved -= this.Tree_NodeRemoved;
                        this.viewModel.MapCollection.NodeMoved -= this.Tree_NodeMoved;
                        this.viewModel.MapCollection.NodeNameChanged -= this.Tree_NodeNameChanged;
                    }
                    this.viewModel = value;
                    if (this.viewModel != null)
                    {
                        this.viewModel.MapCollection.Loaded += this.Tree_Loaded;
                        this.viewModel.MapCollection.Cleared += this.Tree_Cleared;
                        this.viewModel.MapCollection.NodeAdded += this.Tree_NodeAdded;
                        this.viewModel.MapCollection.NodeRemoved += this.Tree_NodeRemoved;
                        this.viewModel.MapCollection.NodeMoved += this.Tree_NodeMoved;
                        this.viewModel.MapCollection.NodeNameChanged += this.Tree_NodeNameChanged;
                    }
                    this.Initialize();
                }
            }
        }
        private ViewModel viewModel;

        private MapCollection MapCollection
        {
            get
            {
                if (this.ViewModel != null)
                {
                    return this.ViewModel.MapCollection;
                }
                else
                {
                    return null;
                }
            }
        }

        private EditorState EditorState
        {
            get
            {
                if (this.ViewModel != null)
                {
                    return this.ViewModel.EditorState;
                }
                else
                {
                    return null;
                }
            }
        }

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
                    this.AddTreeNode(this.Nodes, id);
                }
            }
            this.AllNodes.First(n => (int)n.Tag == this.MapCollection.ProjectNodeId).ImageKey = "Folder";
            this.AllNodes.First(n => (int)n.Tag == this.MapCollection.TrashNodeId).ImageKey = "Bin";
            int selectedId = this.EditorState.SelectedMapId;
            TreeNode selectedNode = this.AllNodes.FirstOrDefault(n => (int)n.Tag == selectedId);
            if (selectedNode != null)
            {
                this.SelectedNode = selectedNode;
            }
        }

        private void AddTreeNode(TreeNodeCollection parentNodes, int id)
        {
            TreeNode node = new TreeNode(this.MapCollection.GetName(id));
            node.ImageKey = "PageWhite";
            node.Tag = id;
            parentNodes.Add(node);
            foreach (int childId in this.MapCollection.GetChildren(id))
            {
                this.AddTreeNode(node.Nodes, childId);
            }
            if (this.MapCollection.IsExpanded(id))
            {
                node.Expand();
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
        }

        private void Tree_NodeNameChanged(object sender, NodeEventArgs e)
        {
            int id = e.NodeId;
            string text = this.MapCollection.GetName(id);
            this.AllNodes.First(n => (int)n.Tag == id).Text = text;
        }

        protected override void OnAfterExpand(TreeViewEventArgs e)
        {
            base.OnAfterExpand(e);
            this.MapCollection.ExpandNode((int)e.Node.Tag);
            this.Invalidate();
        }

        protected override void OnAfterCollapse(TreeViewEventArgs e)
        {
            base.OnAfterCollapse(e);
            this.MapCollection.CollapseNode((int)e.Node.Tag);
            this.Invalidate();
        }

        protected override void OnAfterSelect(TreeViewEventArgs e)
        {
            base.OnAfterSelect(e);
            this.EditorState.SelectedMapId = (int)this.SelectedNode.Tag;
        }

        protected override void OnDrawNode(DrawTreeNodeEventArgs e)
        {
            base.OnDrawNode(e);
            if (!this.Enabled)
            {
                return;
            }
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
                string key = "BulletToggle" + (node.IsExpanded ? "Minus" : "Plus");
                Image toggleImage = this.ImageList.Images[key];
                g.DrawImage(toggleImage,
                    bounds.X + this.Indent * (node.Level - 1) + 1,
                    bounds.Y + 1);
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
            this.contextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.InsertToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.DeleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.EditToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.contextMenuStrip.SuspendLayout();
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
            // contextMenuStrip
            // 
            this.contextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.EditToolStripMenuItem,
            this.toolStripSeparator1,
            this.InsertToolStripMenuItem,
            this.DeleteToolStripMenuItem});
            this.contextMenuStrip.Name = "contextMenuStrip";
            this.contextMenuStrip.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.contextMenuStrip.Size = new System.Drawing.Size(126, 70);
            // 
            // InsertToolStripMenuItem
            // 
            this.InsertToolStripMenuItem.Name = "InsertToolStripMenuItem";
            this.InsertToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.Insert;
            this.InsertToolStripMenuItem.Size = new System.Drawing.Size(125, 22);
            this.InsertToolStripMenuItem.Text = "Insert";
            // 
            // DeleteToolStripMenuItem
            // 
            this.DeleteToolStripMenuItem.Name = "DeleteToolStripMenuItem";
            this.DeleteToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.DeleteToolStripMenuItem.Size = new System.Drawing.Size(125, 22);
            this.DeleteToolStripMenuItem.Text = "Delete";
            // 
            // EditToolStripMenuItem
            // 
            this.EditToolStripMenuItem.Name = "EditToolStripMenuItem";
            this.EditToolStripMenuItem.Size = new System.Drawing.Size(125, 22);
            this.EditToolStripMenuItem.Text = "Edit";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(122, 6);
            // 
            // MapTreeView
            // 
            this.ContextMenuStrip = this.contextMenuStrip;
            this.ImageKey = "PageWhite";
            this.ImageList = this.imageList;
            this.LineColor = System.Drawing.Color.Black;
            this.SelectedImageIndex = 0;
            this.contextMenuStrip.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        private System.ComponentModel.IContainer components;
    }
}
