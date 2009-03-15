namespace Shrimp
{
    partial class MainForm
    {
        /// <summary>
        /// 必要なデザイナ変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナで生成されたコード

        /// <summary>
        /// デザイナ サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディタで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.MainSplitContainer = new System.Windows.Forms.SplitContainer();
            this.LeftSplitContainer = new System.Windows.Forms.SplitContainer();
            this.TileSetPaletteToolStrip = new System.Windows.Forms.ToolStrip();
            this.TileSetsToolStripComboBox = new System.Windows.Forms.ToolStripComboBox();
            this.TileSetPalette = new Shrimp.TileSetPalette();
            this.MapTreeView = new Shrimp.MapTreeView();
            this.MapEditor = new Shrimp.MapEditor();
            this.ToolStripContainer = new System.Windows.Forms.ToolStripContainer();
            this.ToolStrip = new System.Windows.Forms.ToolStrip();
            this.NewToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.OpenToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.CloseToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.SaveToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.PenToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.DatabaseToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.OpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.MainSplitContainer.Panel1.SuspendLayout();
            this.MainSplitContainer.Panel2.SuspendLayout();
            this.MainSplitContainer.SuspendLayout();
            this.LeftSplitContainer.Panel1.SuspendLayout();
            this.LeftSplitContainer.Panel2.SuspendLayout();
            this.LeftSplitContainer.SuspendLayout();
            this.TileSetPaletteToolStrip.SuspendLayout();
            this.ToolStripContainer.ContentPanel.SuspendLayout();
            this.ToolStripContainer.TopToolStripPanel.SuspendLayout();
            this.ToolStripContainer.SuspendLayout();
            this.ToolStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // MainSplitContainer
            // 
            this.MainSplitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MainSplitContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.MainSplitContainer.IsSplitterFixed = true;
            this.MainSplitContainer.Location = new System.Drawing.Point(0, 0);
            this.MainSplitContainer.Name = "MainSplitContainer";
            // 
            // MainSplitContainer.Panel1
            // 
            this.MainSplitContainer.Panel1.Controls.Add(this.LeftSplitContainer);
            // 
            // MainSplitContainer.Panel2
            // 
            this.MainSplitContainer.Panel2.Controls.Add(this.MapEditor);
            this.MainSplitContainer.Size = new System.Drawing.Size(792, 538);
            this.MainSplitContainer.SplitterDistance = 300;
            this.MainSplitContainer.SplitterWidth = 3;
            this.MainSplitContainer.TabIndex = 0;
            // 
            // LeftSplitContainer
            // 
            this.LeftSplitContainer.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LeftSplitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LeftSplitContainer.Location = new System.Drawing.Point(0, 0);
            this.LeftSplitContainer.Name = "LeftSplitContainer";
            this.LeftSplitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // LeftSplitContainer.Panel1
            // 
            this.LeftSplitContainer.Panel1.Controls.Add(this.TileSetPaletteToolStrip);
            this.LeftSplitContainer.Panel1.Controls.Add(this.TileSetPalette);
            // 
            // LeftSplitContainer.Panel2
            // 
            this.LeftSplitContainer.Panel2.Controls.Add(this.MapTreeView);
            this.LeftSplitContainer.Size = new System.Drawing.Size(300, 538);
            this.LeftSplitContainer.SplitterDistance = 300;
            this.LeftSplitContainer.SplitterWidth = 3;
            this.LeftSplitContainer.TabIndex = 0;
            // 
            // TilesPaletteToolStrip
            // 
            this.TileSetPaletteToolStrip.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.TileSetPaletteToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.TileSetPaletteToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.TileSetsToolStripComboBox});
            this.TileSetPaletteToolStrip.Location = new System.Drawing.Point(0, 271);
            this.TileSetPaletteToolStrip.Name = "TilesPaletteToolStrip";
            this.TileSetPaletteToolStrip.Size = new System.Drawing.Size(296, 25);
            this.TileSetPaletteToolStrip.TabIndex = 1;
            // 
            // TileSetsToolStripComboBox
            // 
            this.TileSetsToolStripComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.TileSetsToolStripComboBox.FlatStyle = System.Windows.Forms.FlatStyle.Standard;
            this.TileSetsToolStripComboBox.Name = "TileSetsToolStripComboBox";
            this.TileSetsToolStripComboBox.Size = new System.Drawing.Size(121, 25);
            this.TileSetsToolStripComboBox.SelectedIndexChanged += new System.EventHandler(this.TileSetsToolStripComboBox_SelectedIndexChanged);
            // 
            // TilesPalette
            // 
            this.TileSetPalette.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.TileSetPalette.AutoScroll = true;
            this.TileSetPalette.Location = new System.Drawing.Point(0, 0);
            this.TileSetPalette.Name = "TilesPalette";
            this.TileSetPalette.Size = new System.Drawing.Size(272, 171);
            this.TileSetPalette.TabIndex = 0;
            this.TileSetPalette.ViewModel = null;
            // 
            // MapTreeView
            // 
            this.MapTreeView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.MapTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MapTreeView.DrawMode = System.Windows.Forms.TreeViewDrawMode.OwnerDrawAll;
            this.MapTreeView.FullRowSelect = true;
            this.MapTreeView.HideSelection = false;
            this.MapTreeView.ImageKey = "PageWhite";
            this.MapTreeView.ItemHeight = 21;
            this.MapTreeView.Location = new System.Drawing.Point(0, 0);
            this.MapTreeView.Name = "MapTreeView";
            this.MapTreeView.ShowLines = false;
            this.MapTreeView.ShowRootLines = false;
            this.MapTreeView.Size = new System.Drawing.Size(296, 231);
            this.MapTreeView.TabIndex = 0;
            this.MapTreeView.ViewModel = null;
            // 
            // MapEditor
            // 
            this.MapEditor.AutoScroll = true;
            this.MapEditor.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.MapEditor.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MapEditor.Location = new System.Drawing.Point(0, 0);
            this.MapEditor.Name = "MapEditor";
            this.MapEditor.Size = new System.Drawing.Size(489, 538);
            this.MapEditor.TabIndex = 0;
            this.MapEditor.ViewModel = null;
            // 
            // ToolStripContainer
            // 
            // 
            // ToolStripContainer.BottomToolStripPanel
            // 
            this.ToolStripContainer.BottomToolStripPanel.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            // 
            // ToolStripContainer.ContentPanel
            // 
            this.ToolStripContainer.ContentPanel.Controls.Add(this.MainSplitContainer);
            this.ToolStripContainer.ContentPanel.Size = new System.Drawing.Size(792, 538);
            this.ToolStripContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ToolStripContainer.Location = new System.Drawing.Point(0, 0);
            this.ToolStripContainer.Name = "ToolStripContainer";
            this.ToolStripContainer.Size = new System.Drawing.Size(792, 573);
            this.ToolStripContainer.TabIndex = 1;
            this.ToolStripContainer.Text = "toolStripContainer1";
            // 
            // ToolStripContainer.TopToolStripPanel
            // 
            this.ToolStripContainer.TopToolStripPanel.Controls.Add(this.ToolStrip);
            this.ToolStripContainer.TopToolStripPanel.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            // 
            // ToolStrip
            // 
            this.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
            this.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.ToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.NewToolStripButton,
            this.OpenToolStripButton,
            this.CloseToolStripButton,
            this.SaveToolStripButton,
            this.toolStripSeparator1,
            this.PenToolStripButton,
            this.toolStripSeparator2,
            this.DatabaseToolStripButton});
            this.ToolStrip.Location = new System.Drawing.Point(0, 0);
            this.ToolStrip.Name = "ToolStrip";
            this.ToolStrip.Size = new System.Drawing.Size(792, 35);
            this.ToolStrip.Stretch = true;
            this.ToolStrip.TabIndex = 0;
            // 
            // NewToolStripButton
            // 
            this.NewToolStripButton.Image = global::Shrimp.Properties.Resources.Document;
            this.NewToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.NewToolStripButton.Name = "NewToolStripButton";
            this.NewToolStripButton.Size = new System.Drawing.Size(31, 32);
            this.NewToolStripButton.Text = "New";
            this.NewToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.NewToolStripButton.Click += new System.EventHandler(this.NewToolStripButton_Click);
            // 
            // OpenToolStripButton
            // 
            this.OpenToolStripButton.Image = global::Shrimp.Properties.Resources.FolderOpen;
            this.OpenToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.OpenToolStripButton.Name = "OpenToolStripButton";
            this.OpenToolStripButton.Size = new System.Drawing.Size(35, 32);
            this.OpenToolStripButton.Text = "Open";
            this.OpenToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.OpenToolStripButton.Click += new System.EventHandler(this.OpenToolStripButton_Click);
            // 
            // CloseToolStripButton
            // 
            this.CloseToolStripButton.Image = global::Shrimp.Properties.Resources.Folder;
            this.CloseToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.CloseToolStripButton.Name = "CloseToolStripButton";
            this.CloseToolStripButton.Size = new System.Drawing.Size(38, 32);
            this.CloseToolStripButton.Text = "Close";
            this.CloseToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.CloseToolStripButton.Click += new System.EventHandler(this.CloseToolStripButton_Click);
            // 
            // SaveToolStripButton
            // 
            this.SaveToolStripButton.Image = global::Shrimp.Properties.Resources.Disk;
            this.SaveToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.SaveToolStripButton.Name = "SaveToolStripButton";
            this.SaveToolStripButton.Size = new System.Drawing.Size(34, 32);
            this.SaveToolStripButton.Text = "Save";
            this.SaveToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.SaveToolStripButton.Click += new System.EventHandler(this.SaveToolStripButton_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 35);
            // 
            // PenToolStripButton
            // 
            this.PenToolStripButton.Image = global::Shrimp.Properties.Resources.Pencil;
            this.PenToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.PenToolStripButton.Name = "PenToolStripButton";
            this.PenToolStripButton.Size = new System.Drawing.Size(28, 32);
            this.PenToolStripButton.Text = "Pen";
            this.PenToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 35);
            // 
            // DatabaseToolStripButton
            // 
            this.DatabaseToolStripButton.Image = global::Shrimp.Properties.Resources.Database;
            this.DatabaseToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.DatabaseToolStripButton.Name = "DatabaseToolStripButton";
            this.DatabaseToolStripButton.Size = new System.Drawing.Size(57, 32);
            this.DatabaseToolStripButton.Text = "Database";
            this.DatabaseToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.DatabaseToolStripButton.Click += new System.EventHandler(this.DatabaseToolStripButton_Click);
            // 
            // OpenFileDialog
            // 
            this.OpenFileDialog.Filter = "Project File (*.shrp)|*.shrp|All Files (*.*)|*.*";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(792, 573);
            this.Controls.Add(this.ToolStripContainer);
            this.Name = "MainForm";
            this.Text = "Shrimp";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.MainSplitContainer.Panel1.ResumeLayout(false);
            this.MainSplitContainer.Panel2.ResumeLayout(false);
            this.MainSplitContainer.ResumeLayout(false);
            this.LeftSplitContainer.Panel1.ResumeLayout(false);
            this.LeftSplitContainer.Panel1.PerformLayout();
            this.LeftSplitContainer.Panel2.ResumeLayout(false);
            this.LeftSplitContainer.ResumeLayout(false);
            this.TileSetPaletteToolStrip.ResumeLayout(false);
            this.TileSetPaletteToolStrip.PerformLayout();
            this.ToolStripContainer.ContentPanel.ResumeLayout(false);
            this.ToolStripContainer.TopToolStripPanel.ResumeLayout(false);
            this.ToolStripContainer.TopToolStripPanel.PerformLayout();
            this.ToolStripContainer.ResumeLayout(false);
            this.ToolStripContainer.PerformLayout();
            this.ToolStrip.ResumeLayout(false);
            this.ToolStrip.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer MainSplitContainer;
        private System.Windows.Forms.ToolStripContainer ToolStripContainer;
        private System.Windows.Forms.ToolStrip ToolStrip;
        private System.Windows.Forms.ToolStripButton NewToolStripButton;
        private MapEditor MapEditor;
        private System.Windows.Forms.SplitContainer LeftSplitContainer;
        private System.Windows.Forms.ToolStripButton OpenToolStripButton;
        private System.Windows.Forms.OpenFileDialog OpenFileDialog;
        private System.Windows.Forms.ToolStripButton CloseToolStripButton;
        private TileSetPalette TileSetPalette;
        private System.Windows.Forms.ToolStrip TileSetPaletteToolStrip;
        private System.Windows.Forms.ToolStripButton SaveToolStripButton;
        private MapTreeView MapTreeView;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton DatabaseToolStripButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton PenToolStripButton;
        private System.Windows.Forms.ToolStripComboBox TileSetsToolStripComboBox;
    }
}

