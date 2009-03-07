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
    internal partial class MainForm : Form
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
                items[i].Click += (sender, e) => { this.ViewModel.SelectedTileSetIndex = j; };
            }
            this.ViewModel = new ViewModel();
            this.ViewModel.IsOpenedChanged += delegate
            {
                this.IsOpenedChanged();
            };
            this.ViewModel.IsDirtyChanged += delegate
            {
                this.IsDirtyChanged();
            };
            this.ViewModel.SelectedTileSetIndexChanged += delegate
            {
                this.SelectedTileSetIndexChanged();
            };
            this.ViewModel.Project.Updated += (s, e) =>
            {
                switch (e.PropertyName)
                {
                case "GameTitle":
                    this.GameTitleChanged();
                    break;
                }
            };
            this.MapTreeView.Tree = this.ViewModel.MapCollection;
            this.IsOpenedChanged();
        }

        private ViewModel ViewModel;

        private IEnumerable<ToolStripButton> TilesPaletteSwitchers
        {
            get
            {
                return this.TilesPaletteToolStrip.Items.OfType<ToolStripButton>();
            }
        }

        private void IsOpenedChanged()
        {
            bool isOpened = this.ViewModel.IsOpened;
            this.MapTreeView.Enabled = isOpened;
            this.MapEditor.Enabled = isOpened;
            this.NewToolStripButton.Enabled = !isOpened;
            this.OpenToolStripButton.Enabled = !isOpened;
            this.CloseToolStripButton.Enabled = isOpened;
            this.SaveToolStripButton.Enabled = isOpened && this.ViewModel.IsDirty;
            this.TilesPaletteToolStrip.Enabled = isOpened;
            this.IsDirtyChanged();
            this.SelectedTileSetIndexChanged();
            this.GameTitleChanged();
        }

        private void IsDirtyChanged()
        {
            this.SaveToolStripButton.Enabled =
                this.ViewModel.IsOpened && this.ViewModel.IsDirty;
        }

        private void SelectedTileSetIndexChanged()
        {
            if (this.ViewModel.IsOpened)
            {
                this.TilesPalette.TilesBitmap = this.ViewModel.GetTilesBitmap();
                var items = this.TilesPaletteSwitchers.ToArray();
                for (int i = 0; i < items.Length; i++)
                {
                    items[i].Checked = (i == this.ViewModel.SelectedTileSetIndex);
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

        private void GameTitleChanged()
        {
            if (this.ViewModel.IsOpened)
            {
                this.Text = this.ViewModel.Project.GameTitle + " - Shrimp";
            }
            else
            {
                this.Text = "Shrimp";
            }
        }

        private void NewToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(!this.ViewModel.IsOpened);
            var dialog = new NewProjectDialog();
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                string directoryPath = Path.Combine(dialog.BasePath, dialog.DirectoryName);
                this.ViewModel.New(directoryPath, dialog.GameTitle);
                Debug.Assert(this.ViewModel.IsOpened);
                Debug.Assert(!this.ViewModel.IsDirty);
            }
        }

        private void OpenToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(!this.ViewModel.IsOpened);
            if (this.OpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                string directoryPath = Path.GetDirectoryName(this.OpenFileDialog.FileName);
                this.ViewModel.Open(directoryPath);
                Debug.Assert(this.ViewModel.IsOpened);
                Debug.Assert(!this.ViewModel.IsDirty);
            }
        }

        private void CloseToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.ViewModel.IsOpened);
            this.ViewModel.Close();
            Debug.Assert(!this.ViewModel.IsOpened);
            Debug.Assert(!this.ViewModel.IsDirty);
        }

        private void SaveToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.ViewModel.IsOpened);
            Debug.Assert(this.ViewModel.IsDirty);
            this.ViewModel.Save();
            Debug.Assert(this.ViewModel.IsOpened);
            Debug.Assert(!this.ViewModel.IsDirty);
        }
    }
}
