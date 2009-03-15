﻿using System;
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
            this.SuspendLayout();
            this.ToolStrip.Renderer = new CustomToolStripSystemRenderer();
            this.TilesPaletteToolStrip.Renderer = new CustomToolStripSystemRenderer();
            this.TilesPalette.Height =
                this.TilesPalette.Parent.ClientSize.Height - this.TilesPaletteToolStrip.Height;
            this.MainSplitContainer.SplitterDistance -=
                this.TilesPalette.Parent.ClientSize.Width - this.TilesPalette.Width;

            this.PenToolStripButton.Tag = MapEditorModes.Pen;
            foreach (var item in this.MapEditorModeSwitchers)
            {
                item.Click += (s, e) =>
                {
                    this.ViewModel.EditorState.MapEditorMode =
                        (MapEditorModes)((ToolStripButton)s).Tag;
                };
            }

            this.DatabaseDialog = new DatabaseDialog();
            this.ViewModel = new ViewModel();
            this.ViewModel.IsOpenedChanged += delegate
            {
                this.IsOpenedChanged();
            };
            this.ViewModel.IsDirtyChanged += delegate
            {
                this.IsDirtyChanged();
            };
            this.ViewModel.Project.Updated += (s, e) =>
            {
                switch (e.PropertyName)
                {
                case "GameTitle":
                    this.GameTitleChanged();
                    break;
                default:
                    throw new ArgumentException("Invalid property name", "e");
                }
            };
            this.ViewModel.EditorState.Updated += (s, e) =>
            {
                switch (e.PropertyName)
                {
                case "SelectedMapId":
                    this.SelectedMapIdChanged();
                    break;
                case "MapOffsets":
                    break;
                case "SelectedTileSetIds":
                    if ((int)e.Tag == this.ViewModel.EditorState.SelectedMapId)
                    {
                        this.SelectedTileSetIdsChanged();
                    }
                    break;
                case "MapEditorMode":
                    this.MapEditorModeChanged();
                    break;
                default:
                    throw new ArgumentException("Invalid property name", "e");
                }
            };
            this.MapTreeView.ViewModel = this.ViewModel;
            this.MapEditor.ViewModel = this.ViewModel;
            this.TilesPalette.ViewModel = this.ViewModel;

            this.IsOpenedChanged();
            this.ResumeLayout(false);
        }

        private ViewModel ViewModel;

        private DatabaseDialog DatabaseDialog;

        private IEnumerable<ToolStripButton> TilesPaletteSwitchers
        {
            get
            {
                return this.TilesPaletteToolStrip.Items.OfType<ToolStripButton>();
            }
        }

        private IEnumerable<ToolStripButton> MapEditorModeSwitchers
        {
            get
            {
                yield return this.PenToolStripButton;
            }
        }

        private void IsOpenedChanged()
        {
            bool isOpened = this.ViewModel.IsOpened;

            this.TileSetsToolStripComboBox.Items.Clear();
            if (isOpened)
            {
                this.TileSetsToolStripComboBox.BeginUpdate();
                var tileSets = this.ViewModel.TileSetCollection.Items.ToArray();
                Dictionary<int, int> indexToId = new Dictionary<int, int>();
                for (int i = 0; i < tileSets.Length; i++)
                {
                    TileSet tileSet = tileSets[i];
                    indexToId.Add(i, tileSet.Id);
                    this.TileSetsToolStripComboBox.Items.Add(tileSet.ImageFileName);
                }
                this.TileSetsToolStripComboBox.Tag = indexToId;
                this.TileSetsToolStripComboBox.EndUpdate();
            }

            this.MapTreeView.Enabled = isOpened;
            this.MapEditor.Enabled = isOpened;
            this.NewToolStripButton.Enabled = !isOpened;
            this.OpenToolStripButton.Enabled = !isOpened;
            this.CloseToolStripButton.Enabled = isOpened;
            this.SaveToolStripButton.Enabled = isOpened;
            foreach (var item in this.MapEditorModeSwitchers)
            {
                item.Enabled = isOpened;
            }
            this.DatabaseToolStripButton.Enabled = isOpened;
            this.TilesPaletteToolStrip.Enabled = isOpened;
            this.IsDirtyChanged();
            this.GameTitleChanged();
            this.SelectedTileSetIdsChanged();
            this.SelectedTileSetIdsChanged();
            this.MapEditorModeChanged();
        }

        private void IsDirtyChanged()
        {
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

        private void SelectedMapIdChanged()
        {
            if (this.ViewModel.IsOpened)
            {
                int mapId = this.ViewModel.EditorState.SelectedMapId;
                int tileSetId = this.ViewModel.EditorState.GetSelectedTileSetId(mapId);
                var indexToId = (Dictionary<int, int>)this.TileSetsToolStripComboBox.Tag;
                int index = (from p in indexToId
                             where p.Value == tileSetId
                             select p.Key).FirstOrDefault();
                this.TileSetsToolStripComboBox.SelectedIndex = index;
            }
        }

        private void SelectedTileSetIdsChanged()
        {
            if (this.ViewModel.IsOpened)
            {
                int mapId = this.ViewModel.EditorState.SelectedMapId;
                int tileSetId = this.ViewModel.EditorState.GetSelectedTileSetId(mapId);
                var indexToId = (Dictionary<int, int>)this.TileSetsToolStripComboBox.Tag;
                int index = (from p in indexToId
                             where p.Value == tileSetId
                             select p.Key).FirstOrDefault();
                this.TileSetsToolStripComboBox.SelectedIndex = index;
            }
        }

        private void MapEditorModeChanged()
        {
            if (this.ViewModel.IsOpened)
            {
                foreach (var item in this.MapEditorModeSwitchers)
                {
                    item.Checked =
                        ((MapEditorModes)item.Tag == this.ViewModel.EditorState.MapEditorMode);
                }
            }
            else
            {
                foreach (var item in this.MapEditorModeSwitchers)
                {
                    item.Checked = false;
                }
            }
        }

        private void NewToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(!this.ViewModel.IsOpened);
            using (var dialog = new NewProjectDialog())
            {
                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    string directoryPath = Path.Combine(dialog.BasePath, dialog.DirectoryName);
                    this.ViewModel.New(directoryPath, dialog.GameTitle);
                    Debug.Assert(this.ViewModel.IsOpened);
                    Debug.Assert(!this.ViewModel.IsDirty);
                }
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
                // Debug.Assert(!this.ViewModel.IsDirty);
            }
        }

        private void CloseToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.ViewModel.IsOpened);
            if (this.ViewModel.IsDirty)
            {
                DialogResult result = MessageBox.Show("Save?", "",
                    MessageBoxButtons.YesNoCancel,
                    MessageBoxIcon.Question,
                    MessageBoxDefaultButton.Button1);
                switch (result)
                {
                case DialogResult.Yes:
                    this.ViewModel.Save();
                    break;
                case DialogResult.No:
                    break;
                case DialogResult.Cancel:
                    return;
                }
            }
            this.ViewModel.Close();
            Debug.Assert(!this.ViewModel.IsOpened);
            Debug.Assert(!this.ViewModel.IsDirty);
        }

        private void SaveToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.ViewModel.IsOpened);
            if (this.ViewModel.IsDirty)
            {
                this.ViewModel.Save();
            }
            Debug.Assert(this.ViewModel.IsOpened);
            Debug.Assert(!this.ViewModel.IsDirty);
        }

        private void DatabaseToolStripButton_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.ViewModel.IsOpened);
            this.DatabaseDialog.ShowDialog();
            Debug.Assert(this.ViewModel.IsOpened);
        }

        private void TileSetsToolStripComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            int selectedIndex = this.TileSetsToolStripComboBox.SelectedIndex;
            if (selectedIndex != -1)
            {
                int mapId = this.ViewModel.EditorState.SelectedMapId;
                var indexToId = (Dictionary<int, int>)this.TileSetsToolStripComboBox.Tag;
                int tileSetId = indexToId[selectedIndex];
                this.ViewModel.EditorState.SetSelectedTileSetId(mapId, tileSetId);
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this.ViewModel.IsDirty)
            {
                DialogResult result = MessageBox.Show("Save?", "",
                    MessageBoxButtons.YesNoCancel,
                    MessageBoxIcon.Question,
                    MessageBoxDefaultButton.Button1);
                switch (result)
                {
                case DialogResult.Yes:
                    this.ViewModel.Save();
                    break;
                case DialogResult.No:
                    break;
                case DialogResult.Cancel:
                    e.Cancel = true;
                    return;
                }
            }
        }
    }
}
