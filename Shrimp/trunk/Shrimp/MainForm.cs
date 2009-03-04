using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    public partial class MainForm : Form
    {
        private class CustomToolStripSystemRenderer : ToolStripSystemRenderer
        {
            protected override void OnRenderToolStripBorder(ToolStripRenderEventArgs e)
            {
                // Do nothing
            }
        }

        public MainForm()
        {
            this.InitializeComponent();
            this.ToolStrip.Renderer = new CustomToolStripSystemRenderer();
            this.TilesPaletteToolStrip.Renderer = new CustomToolStripSystemRenderer();
            this.TilesPalette.Height =
                this.TilesPalette.Parent.ClientSize.Height - this.TilesPaletteToolStrip.Height;
            this.MainSplitContainer.SplitterDistance -=
                this.TilesPalette.Parent.ClientSize.Width - this.TilesPalette.Width;
            var items = this.TilesPaletteSwitchers.ToArray();
            for (int i = 0; i < items.Length; i++)
            {
                var j = i;
                items[i].Click += (sender, e) => { this.PaletteIndex = j; };
            }
            this.UpdateState();
        }

        private IEnumerable<ToolStripButton> TilesPaletteSwitchers
        {
            get
            {
                return this.TilesPaletteToolStrip.Items.OfType<ToolStripButton>();
            }
        }

        private void UpdateState()
        {
            bool hasProject = this.ProjectDirectoryPath != null;
            this.MapsTreeView.Enabled = hasProject;
            this.MapEditor.Enabled = hasProject;
            this.NewToolStripButton.Enabled = !hasProject;
            this.OpenToolStripButton.Enabled = !hasProject;
            this.CloseToolStripButton.Enabled = hasProject;
            this.TilesPaletteToolStrip.Enabled = hasProject;
            if (hasProject)
            {
                string tilesBitmapPath =
                    Path.Combine(Path.Combine(this.ProjectDirectoryPath, "Graphics"), "Tiles.png");
                this.TilesPalette.TilesBitmap = Bitmap.FromFile(tilesBitmapPath) as Bitmap;
                var items = this.TilesPaletteSwitchers.ToArray();
                for (int i = 0; i < items.Length; i++)
                {
                    items[i].Checked = (i == this.PaletteIndex);
                }
            }
            else
            {
                this.TilesPalette.TilesBitmap = null;
                foreach (var item in this.TilesPaletteSwitchers)
                {
                    item.Checked = false;
                }
            }
        }

        private string ProjectDirectoryPath
        {
            get
            {
                return this.projectDirectoryPath;
            }
            set
            {
                if (this.projectDirectoryPath != value)
                {
                    this.projectDirectoryPath = value;
                    this.UpdateState();
                }
            }
        }
        private string projectDirectoryPath;

        private int PaletteIndex
        {
            get
            {
                return this.paletteIndex;
            }
            set
            {
                if (this.paletteIndex != value)
                {
                    this.paletteIndex = value;
                    this.UpdateState();
                }
            }
        }
        private int paletteIndex;

        private void NewToolStripButton_Click(object sender, EventArgs e)
        {
            var dialog = new NewProjectDialog();
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                string path = Path.Combine(dialog.BasePath, dialog.DirectoryName);
                Debug.Assert(!Directory.Exists(path));
                CopyDirectory("ProjectTemplate", path);
                this.PaletteIndex = 0;
                this.ProjectDirectoryPath = path;
            }
        }

        private static void CopyDirectory(string src, string dst)
        {
            if (!Directory.Exists(dst))
            {
                Directory.CreateDirectory(dst);
                File.SetAttributes(dst, File.GetAttributes(src));
            }
            foreach (string file in Directory.GetFiles(src))
            {
                string dstFile = Path.Combine(dst, Path.GetFileName(file));
                File.Copy(file, dstFile);
                File.SetAttributes(dstFile, File.GetAttributes(file));
            }
            foreach (string dir in Directory.GetDirectories(src))
            {
                CopyDirectory(dir, Path.Combine(dst, Path.GetFileName(dir)));
            }
        }

        private void OpenToolStripButton_Click(object sender, EventArgs e)
        {
            if (this.OpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                this.PaletteIndex = 0;
                this.ProjectDirectoryPath =
                    Path.GetDirectoryName(this.OpenFileDialog.FileName);
            }
        }

        private void CloseToolStripButton_Click(object sender, EventArgs e)
        {
            this.ProjectDirectoryPath = null;
        }
    }
}
