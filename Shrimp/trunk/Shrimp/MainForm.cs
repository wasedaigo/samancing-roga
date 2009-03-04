using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            this.TilesPalette.Height = this.TilesPalette.Parent.ClientSize.Height;
            this.MainSplitContainer.SplitterDistance -=
                this.TilesPalette.Parent.ClientSize.Width - this.TilesPalette.Width;
            this.UpdateState();
        }

        private void UpdateState()
        {
            bool hasProject = this.ProjectDirectoryPath != null;
            //this.MapsTreeView.Enabled = hasProject;
            this.MapEditor.Enabled = hasProject;
            this.NewToolStripButton.Enabled = !hasProject;
            this.OpenToolStripButton.Enabled = !hasProject;
            this.CloseToolStripButton.Enabled = hasProject;
            if (hasProject)
            {
                string tilesBitmapPath =
                    Path.Combine(Path.Combine(this.ProjectDirectoryPath, "Graphics"), "Tiles.png");
                this.TilesPalette.TilesBitmap = Bitmap.FromFile(tilesBitmapPath) as Bitmap;
            }
            else
            {
                this.TilesPalette.TilesBitmap = null;
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

        private void NewToolStripButton_Click(object sender, EventArgs e)
        {
            var d = new NewProjectDialog();
            if (d.ShowDialog() == DialogResult.OK)
            {
                string basePath = d.BasePath;
                string directoryName = d.DirectoryName;
                string path = Path.Combine(basePath, directoryName);
                if (Directory.Exists(path))
                {
                    throw new Exception("directory already exists");
                }
                CopyDirectory("ProjectTemplate", path);
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
                File.Copy(file, Path.Combine(dst, Path.GetFileName(file)));
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
                string path = Path.GetDirectoryName(this.OpenFileDialog.FileName);
                this.ProjectDirectoryPath = path;
            }
        }

        private void CloseToolStripButton_Click(object sender, EventArgs e)
        {
            this.ProjectDirectoryPath = null;
        }
    }
}
