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
                items[i].Click += (sender, e) => { this.ViewModel.TileSetIndex = j; };
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
            bool hasViewModel = (this.ViewModel != null);
            this.MapsTreeView.Enabled = hasViewModel;
            this.MapEditor.Enabled = hasViewModel;
            this.NewToolStripButton.Enabled = !hasViewModel;
            this.OpenToolStripButton.Enabled = !hasViewModel;
            this.CloseToolStripButton.Enabled = hasViewModel;
            this.SaveToolStripButton.Enabled = hasViewModel;
            this.TilesPaletteToolStrip.Enabled = hasViewModel;
            if (hasViewModel)
            {
                this.Text = this.ViewModel.GameTitle + " - Shrimp";
                this.TilesPalette.TilesBitmap = this.ViewModel.GetTilesBitmap();
                var items = this.TilesPaletteSwitchers.ToArray();
                for (int i = 0; i < items.Length; i++)
                {
                    items[i].Checked = (i == this.ViewModel.TileSetIndex);
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

        private ViewModel ViewModel
        {
            get
            {
                return this.viewModel;
            }
            set
            {
                if (this.viewModel != value)
                {
                    if (this.viewModel != null)
                    {
                        this.viewModel.Updated -= this.Project_Updated;
                        this.MapsTreeView.Nodes.Clear();
                    }
                    this.viewModel = value;
                    if (this.viewModel != null)
                    {
                        this.viewModel.Updated += this.Project_Updated;
                        this.MapsTreeView.Nodes.Add(this.ViewModel.GameTitle);
                    }
                    this.UpdateState();
                }
            }
        }
        private ViewModel viewModel;

        private void Project_Updated(object sender, EventArgs e)
        {
            this.UpdateState();
        }

        private void NewToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.ViewModel == null);
            var dialog = new NewProjectDialog();
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                string directoryPath = Path.Combine(dialog.BasePath, dialog.DirectoryName);
                ViewModel viewModel = new ViewModel(directoryPath);
                viewModel.GameTitle = dialog.GameTitle;
                viewModel.InitializeAndSave();
                this.ViewModel = viewModel;
            }
        }

        private void OpenToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.ViewModel == null);
            if (this.OpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                string directoryPath = Path.GetDirectoryName(this.OpenFileDialog.FileName);
                ViewModel viewModel = new ViewModel(directoryPath);
                viewModel.Load();
                this.ViewModel = viewModel;
            }
        }

        private void CloseToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.ViewModel != null);
            this.ViewModel = null;
        }

        private void SaveToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.ViewModel != null);
            this.ViewModel.Save();
        }
    }
}
