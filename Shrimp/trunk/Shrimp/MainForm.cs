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
using System.Xml;

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
            bool hasProject = (this.Project != null);
            this.MapsTreeView.Enabled = hasProject;
            this.MapEditor.Enabled = hasProject;
            this.NewToolStripButton.Enabled = !hasProject;
            this.OpenToolStripButton.Enabled = !hasProject;
            this.CloseToolStripButton.Enabled = hasProject;
            this.TilesPaletteToolStrip.Enabled = hasProject;
            if (hasProject)
            {
                this.Text = this.Project.GameTitle + " - Shrimp";
                string directoryPath =
                    Path.Combine(this.Project.BasePath, this.Project.DirectoryName);
                string tilesBitmapPath =
                    Path.Combine(Path.Combine(directoryPath, "Graphics"), "Tiles.png");
                this.TilesPalette.TilesBitmap = Bitmap.FromFile(tilesBitmapPath) as Bitmap;
                var items = this.TilesPaletteSwitchers.ToArray();
                for (int i = 0; i < items.Length; i++)
                {
                    items[i].Checked = (i == this.PaletteIndex);
                }
            }
            else
            {
                this.Text = "Shrimp";
                this.TilesPalette.TilesBitmap = null;
                foreach (var item in this.TilesPaletteSwitchers)
                {
                    item.Checked = false;
                }
            }
        }

        private Project Project
        {
            get
            {
                return this.project;
            }
            set
            {
                if (this.project != value)
                {
                    if (this.project != null)
                    {
                        this.project.Updated -= this.Project_Updated;
                    }
                    this.project = value;
                    if (this.project != null)
                    {
                        this.project.Updated += this.Project_Updated;
                    }
                    this.UpdateState();
                }
            }
        }
        private Project project;

        private void Project_Updated(object sender, EventArgs e)
        {
            this.UpdateState();
        }

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
                Project project = new Project();
                project.BasePath = dialog.BasePath;
                project.DirectoryName = dialog.DirectoryName;
                project.GameTitle = dialog.GameTitle;
                project.Save();
                this.Project = project;
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
                Project project = new Project();
                project.Load(this.OpenFileDialog.FileName);
                this.Project = project;
            }
        }

        private void CloseToolStripButton_Click(object sender, EventArgs e)
        {
            this.Project = null;
        }
    }
}
