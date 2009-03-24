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

            this.MapEditor = new MapEditor();
            this.MapEditor.BorderStyle = BorderStyle.Fixed3D;
            this.MapEditor.Dock = DockStyle.Fill;
            this.MainSplitContainer.Panel2.Controls.Add(this.MapEditor);

            ToolStripTrackBar tsTrackBar = new ToolStripTrackBar();
            tsTrackBar.Name = "ScaleToolStripTrackBar";
            int index = this.ToolStrip.Items.IndexOf(this.toolStripSeparator5);
            this.ToolStrip.Items.Insert(index, tsTrackBar);
            Debug.Assert(this.ScaleTrackBar == tsTrackBar.TrackBar);

            this.ScaleTrackBar.AutoSize = false;
            this.ScaleTrackBar.Width = 100;
            this.ScaleTrackBar.Height = 20;
            this.ScaleTrackBar.Maximum = 3;
            this.ScaleTrackBar.SmallChange = 1;
            this.ScaleTrackBar.LargeChange = 1;
            tsTrackBar.Text = "Scale";
            tsTrackBar.ToolTipText = "Scale";

            this.ToolStrip.Renderer = new CustomToolStripSystemRenderer();
            this.TileSetPaletteToolStrip.Renderer = new CustomToolStripSystemRenderer();
            this.TileSetPalette.Size = new Size
            {
                Width = Util.BackgroundGridSize * Util.PaletteHorizontalCount
                    + SystemInformation.VerticalScrollBarWidth,
                Height = this.TileSetPalette.Parent.ClientSize.Height
                    - this.TileSetPaletteToolStrip.Height,
            };
            this.MainSplitContainer.SplitterDistance -=
                this.TileSetPalette.Parent.ClientSize.Width - this.TileSetPalette.Width;

            this.Layer1ToolStripButton.Tag = LayerMode.Layer1;
            this.Layer2ToolStripButton.Tag = LayerMode.Layer2;
            this.EventToolStripButton.Tag = LayerMode.Event;
            foreach (var item in this.LayerModeSwitchers)
            {
                item.Click += (s, e) =>
                {
                    this.ViewModel.EditorState.LayerMode =
                        (LayerMode)((ToolStripButton)s).Tag;
                };
            }

            this.PenToolStripButton.Tag = DrawingMode.Pen;
            foreach (var item in this.DrawingModeSwitchers)
            {
                item.Click += (s, e) =>
                {
                    this.ViewModel.EditorState.DrawingMode =
                        (DrawingMode)((ToolStripButton)s).Tag;
                };
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
            this.ViewModel.Project.Updated += (s, e) =>
            {
                switch (e.PropertyName)
                {
                case "GameTitle":
                    this.GameTitleChanged();
                    break;
                }
            };
            this.ViewModel.EditorState.Updated += (s, e) =>
            {
                switch (e.PropertyName)
                {
                case "SelectedMapId":
                    this.SelectedMapIdChanged();
                    break;
                case "SelectedTileSetIds":
                    if (e.ItemId == this.ViewModel.EditorState.SelectedMapId)
                    {
                        this.SelectedTileSetIdsChanged();
                    }
                    break;
                case "LayerMode":
                    this.LayerModeChanged();
                    break;
                case "DrawingMode":
                    this.DrawingModeChanged();
                    break;
                }
            };
            this.MapTreeView.ViewModel = this.ViewModel;
            this.MapEditor.ViewModel = this.ViewModel;
            this.TileSetPalette.ViewModel = this.ViewModel;

            this.IsOpenedChanged();
            this.ResumeLayout(false);
        }

        private ViewModel ViewModel;

        private DatabaseDialog DatabaseDialog = new DatabaseDialog();

        private MapEditor MapEditor;

        private ToolStripTrackBar ScaleToolStripTrackBar
        {
            get
            {
                return (ToolStripTrackBar)this.ToolStrip.Items.Find("ScaleToolStripTrackBar", false)[0];
            }
        }

        private TrackBar ScaleTrackBar
        {
            get
            {
                return this.ScaleToolStripTrackBar.TrackBar;
            }
        }

        private IEnumerable<ToolStripButton> LayerModeSwitchers
        {
            get
            {
                yield return this.Layer1ToolStripButton;
                yield return this.Layer2ToolStripButton;
                yield return this.EventToolStripButton;
            }
        }

        private IEnumerable<ToolStripButton> DrawingModeSwitchers
        {
            get
            {
                yield return this.PenToolStripButton;
            }
        }

        private void IsOpenedChanged()
        {
            bool isOpened = this.ViewModel.IsOpened;
            Debug.Assert((isOpened == true && !this.ViewModel.IsDirty) || isOpened == false);

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
            this.NewToolStripButton.Enabled = !isOpened;
            this.OpenToolStripButton.Enabled = !isOpened;
            this.CloseToolStripButton.Enabled = isOpened;
            this.SaveToolStripButton.Enabled = isOpened;
            this.UndoToolStripButton.Enabled = isOpened;
            foreach (var item in this.LayerModeSwitchers)
            {
                item.Enabled = isOpened;
            }
            foreach (var item in this.DrawingModeSwitchers)
            {
                item.Enabled = isOpened;
            }
            this.ScaleToolStripTrackBar.Enabled = isOpened;
            this.DatabaseToolStripButton.Enabled = isOpened;
            this.TileSetPalette.Enabled = isOpened;
            this.TileSetPaletteToolStrip.Enabled = isOpened;

            this.IsDirtyChanged();
            this.GameTitleChanged();
            this.SelectedMapIdChanged();
            this.SelectedTileSetIdsChanged();
            this.LayerModeChanged();
            this.DrawingModeChanged();

            // To prevent the map editor from being edited wrongly
            Application.DoEvents();
            this.MapEditor.Enabled = isOpened;
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
            this.AdjustTileSetsToolStripComboBox();
            this.TileSetsToolStripComboBox.Enabled =
                this.ViewModel.EditorState.SelectedMap != null;
        }

        private void SelectedTileSetIdsChanged()
        {
            this.AdjustTileSetsToolStripComboBox();
        }

        private void AdjustTileSetsToolStripComboBox()
        {
            if (this.ViewModel.IsOpened)
            {
                int tileSetId = this.ViewModel.EditorState.SelectedTileSetId;
                var indexToId = (Dictionary<int, int>)this.TileSetsToolStripComboBox.Tag;
                int index = (from p in indexToId
                             where p.Value == tileSetId
                             select p.Key).FirstOrDefault();
                this.TileSetsToolStripComboBox.SelectedIndex = index;
            }
        }

        private void LayerModeChanged()
        {
            if (this.ViewModel.IsOpened)
            {
                foreach (var item in this.LayerModeSwitchers)
                {
                    item.Checked =
                        ((LayerMode)item.Tag == this.ViewModel.EditorState.LayerMode);
                }
            }
            else
            {
                foreach (var item in this.LayerModeSwitchers)
                {
                    item.Checked = false;
                }
            }
        }

        private void DrawingModeChanged()
        {
            if (this.ViewModel.IsOpened)
            {
                foreach (var item in this.DrawingModeSwitchers)
                {
                    item.Checked =
                        ((DrawingMode)item.Tag == this.ViewModel.EditorState.DrawingMode);
                }
            }
            else
            {
                foreach (var item in this.DrawingModeSwitchers)
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
                Map map = this.ViewModel.EditorState.SelectedMap;
                if (map != null)
                {
                    int mapId = map.Id;
                    var indexToId = (Dictionary<int, int>)this.TileSetsToolStripComboBox.Tag;
                    int tileSetId = indexToId[selectedIndex];
                    this.ViewModel.EditorState.SetSelectedTileSetId(mapId, tileSetId);
                }
                if (this.TileSetsToolStripComboBox.Focused)
                {
                    this.TileSetPalette.Focus();
                }
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

    internal class ToolStripTrackBar : ToolStripControlHost
    {
        public ToolStripTrackBar()
            : base(new TrackBar())
        {
        }

        public TrackBar TrackBar
        {
            get
            {
                return (TrackBar)this.Control;
            }
        }
    }
}
