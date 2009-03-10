namespace Shrimp
{
    partial class DatabaseDialog
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
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.TileSetTabPage = new System.Windows.Forms.TabPage();
            this.OKButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.ApplyButton = new System.Windows.Forms.Button();
            this.tileSetEditor1 = new Shrimp.TileSetEditor();
            this.tabControl1.SuspendLayout();
            this.TileSetTabPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.TileSetTabPage);
            this.tabControl1.Location = new System.Drawing.Point(8, 8);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(816, 632);
            this.tabControl1.TabIndex = 0;
            // 
            // TileSetTabPage
            // 
            this.TileSetTabPage.Controls.Add(this.tileSetEditor1);
            this.TileSetTabPage.Location = new System.Drawing.Point(4, 21);
            this.TileSetTabPage.Name = "TileSetTabPage";
            this.TileSetTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.TileSetTabPage.Size = new System.Drawing.Size(808, 607);
            this.TileSetTabPage.TabIndex = 0;
            this.TileSetTabPage.Text = "Tile Set";
            this.TileSetTabPage.UseVisualStyleBackColor = true;
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(568, 648);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(80, 24);
            this.OKButton.TabIndex = 1;
            this.OKButton.Text = "OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.Location = new System.Drawing.Point(656, 648);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(80, 24);
            this.cancelButton.TabIndex = 2;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // ApplyButton
            // 
            this.ApplyButton.Location = new System.Drawing.Point(744, 648);
            this.ApplyButton.Name = "ApplyButton";
            this.ApplyButton.Size = new System.Drawing.Size(80, 24);
            this.ApplyButton.TabIndex = 3;
            this.ApplyButton.Text = "&Apply";
            this.ApplyButton.UseVisualStyleBackColor = true;
            // 
            // tileSetEditor1
            // 
            this.tileSetEditor1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tileSetEditor1.Location = new System.Drawing.Point(3, 3);
            this.tileSetEditor1.Name = "tileSetEditor1";
            this.tileSetEditor1.Size = new System.Drawing.Size(802, 601);
            this.tileSetEditor1.TabIndex = 0;
            // 
            // DatabaseDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.ClientSize = new System.Drawing.Size(932, 758);
            this.Controls.Add(this.ApplyButton);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.OKButton);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DatabaseDialog";
            this.Padding = new System.Windows.Forms.Padding(0, 0, 6, 6);
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Database";
            this.tabControl1.ResumeLayout(false);
            this.TileSetTabPage.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage TileSetTabPage;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.Button ApplyButton;
        private TileSetEditor tileSetEditor1;
    }
}