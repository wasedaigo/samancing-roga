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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.MainSplitContainer = new System.Windows.Forms.SplitContainer();
            this.LeftSplitContainer = new System.Windows.Forms.SplitContainer();
            this.TilesPaletteToolStrip = new System.Windows.Forms.ToolStrip();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton2 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton3 = new System.Windows.Forms.ToolStripButton();
            this.TilesPalette = new Shrimp.TilesPalette();
            this.MapsTreeView = new System.Windows.Forms.TreeView();
            this.MapEditor = new Shrimp.MapEditor();
            this.ToolStripContainer = new System.Windows.Forms.ToolStripContainer();
            this.ToolStrip = new System.Windows.Forms.ToolStrip();
            this.NewToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.OpenToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.CloseToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.OpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.SaveToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.MainSplitContainer.Panel1.SuspendLayout();
            this.MainSplitContainer.Panel2.SuspendLayout();
            this.MainSplitContainer.SuspendLayout();
            this.LeftSplitContainer.Panel1.SuspendLayout();
            this.LeftSplitContainer.Panel2.SuspendLayout();
            this.LeftSplitContainer.SuspendLayout();
            this.TilesPaletteToolStrip.SuspendLayout();
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
            this.LeftSplitContainer.Panel1.BackColor = System.Drawing.SystemColors.ControlDark;
            this.LeftSplitContainer.Panel1.Controls.Add(this.TilesPaletteToolStrip);
            this.LeftSplitContainer.Panel1.Controls.Add(this.TilesPalette);
            // 
            // LeftSplitContainer.Panel2
            // 
            this.LeftSplitContainer.Panel2.Controls.Add(this.MapsTreeView);
            this.LeftSplitContainer.Size = new System.Drawing.Size(300, 538);
            this.LeftSplitContainer.SplitterDistance = 300;
            this.LeftSplitContainer.SplitterWidth = 3;
            this.LeftSplitContainer.TabIndex = 0;
            // 
            // TilesPaletteToolStrip
            // 
            this.TilesPaletteToolStrip.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.TilesPaletteToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.TilesPaletteToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1,
            this.toolStripButton2,
            this.toolStripButton3});
            this.TilesPaletteToolStrip.Location = new System.Drawing.Point(0, 271);
            this.TilesPaletteToolStrip.Name = "TilesPaletteToolStrip";
            this.TilesPaletteToolStrip.Size = new System.Drawing.Size(296, 25);
            this.TilesPaletteToolStrip.TabIndex = 1;
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton1.Text = "A";
            // 
            // toolStripButton2
            // 
            this.toolStripButton2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButton2.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton2.Image")));
            this.toolStripButton2.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton2.Name = "toolStripButton2";
            this.toolStripButton2.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton2.Text = "B";
            // 
            // toolStripButton3
            // 
            this.toolStripButton3.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButton3.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton3.Image")));
            this.toolStripButton3.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton3.Name = "toolStripButton3";
            this.toolStripButton3.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton3.Text = "C";
            // 
            // TilesPalette
            // 
            this.TilesPalette.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.TilesPalette.AutoScroll = true;
            this.TilesPalette.Location = new System.Drawing.Point(0, 0);
            this.TilesPalette.Name = "TilesPalette";
            this.TilesPalette.Size = new System.Drawing.Size(272, 171);
            this.TilesPalette.TabIndex = 0;
            this.TilesPalette.TilesBitmap = null;
            // 
            // MapsTreeView
            // 
            this.MapsTreeView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.MapsTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MapsTreeView.Location = new System.Drawing.Point(0, 0);
            this.MapsTreeView.Name = "MapsTreeView";
            this.MapsTreeView.Size = new System.Drawing.Size(296, 231);
            this.MapsTreeView.TabIndex = 0;
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
            this.SaveToolStripButton});
            this.ToolStrip.Location = new System.Drawing.Point(0, 0);
            this.ToolStrip.Name = "ToolStrip";
            this.ToolStrip.Size = new System.Drawing.Size(792, 35);
            this.ToolStrip.Stretch = true;
            this.ToolStrip.TabIndex = 0;
            // 
            // NewToolStripButton
            // 
            this.NewToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("NewToolStripButton.Image")));
            this.NewToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.NewToolStripButton.Name = "NewToolStripButton";
            this.NewToolStripButton.Size = new System.Drawing.Size(31, 32);
            this.NewToolStripButton.Text = "New";
            this.NewToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.NewToolStripButton.Click += new System.EventHandler(this.NewToolStripButton_Click);
            // 
            // OpenToolStripButton
            // 
            this.OpenToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("OpenToolStripButton.Image")));
            this.OpenToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.OpenToolStripButton.Name = "OpenToolStripButton";
            this.OpenToolStripButton.Size = new System.Drawing.Size(35, 32);
            this.OpenToolStripButton.Text = "Open";
            this.OpenToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.OpenToolStripButton.Click += new System.EventHandler(this.OpenToolStripButton_Click);
            // 
            // CloseToolStripButton
            // 
            this.CloseToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("CloseToolStripButton.Image")));
            this.CloseToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.CloseToolStripButton.Name = "CloseToolStripButton";
            this.CloseToolStripButton.Size = new System.Drawing.Size(38, 32);
            this.CloseToolStripButton.Text = "Close";
            this.CloseToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.CloseToolStripButton.Click += new System.EventHandler(this.CloseToolStripButton_Click);
            // 
            // OpenFileDialog
            // 
            this.OpenFileDialog.Filter = "Project File|*.shrp|All Files|*.*";
            // 
            // SaveToolStripButton
            // 
            this.SaveToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("SaveToolStripButton.Image")));
            this.SaveToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.SaveToolStripButton.Name = "SaveToolStripButton";
            this.SaveToolStripButton.Size = new System.Drawing.Size(34, 32);
            this.SaveToolStripButton.Text = "Save";
            this.SaveToolStripButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.SaveToolStripButton.Click += new System.EventHandler(this.SaveToolStripButton_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(792, 573);
            this.Controls.Add(this.ToolStripContainer);
            this.Name = "MainForm";
            this.Text = "Shrimp";
            this.MainSplitContainer.Panel1.ResumeLayout(false);
            this.MainSplitContainer.Panel2.ResumeLayout(false);
            this.MainSplitContainer.ResumeLayout(false);
            this.LeftSplitContainer.Panel1.ResumeLayout(false);
            this.LeftSplitContainer.Panel1.PerformLayout();
            this.LeftSplitContainer.Panel2.ResumeLayout(false);
            this.LeftSplitContainer.ResumeLayout(false);
            this.TilesPaletteToolStrip.ResumeLayout(false);
            this.TilesPaletteToolStrip.PerformLayout();
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
        private TilesPalette TilesPalette;
        private System.Windows.Forms.ToolStrip TilesPaletteToolStrip;
        private System.Windows.Forms.ToolStripButton toolStripButton1;
        private System.Windows.Forms.ToolStripButton toolStripButton2;
        private System.Windows.Forms.ToolStripButton toolStripButton3;
        private System.Windows.Forms.TreeView MapsTreeView;
        private System.Windows.Forms.ToolStripButton SaveToolStripButton;
    }
}

