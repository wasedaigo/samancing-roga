using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    internal partial class MapEditor : UserControl
    {
        public MapEditor()
        {
            this.InitializeComponent();
        }

        public ViewModel ViewModel
        {
            get { return this.viewModel; }
            set
            {
                if (this.viewModel != value)
                {
                    if (this.viewModel != null)
                    {
                        this.viewModel.Opened -= this.ViewModel_Opened;
                        this.viewModel.EditorState.Updated -= this.EditorState_Updated;
                    }
                    this.viewModel = value;
                    if (this.viewModel != null)
                    {
                        this.Map = this.viewModel.EditorState.SelectedMap;
                        this.viewModel.Opened += this.ViewModel_Opened;
                        this.viewModel.EditorState.Updated += this.EditorState_Updated;
                    }
                    else
                    {
                        this.Map = null;
                    }
                }
            }
        }
        private ViewModel viewModel;

        private Map Map
        {
            get { return this.map; }
            set
            {
                if (this.map != value)
                {
                    if (this.map != null)
                    {
                        this.map.Updated -= this.Map_Updated;
                    }
                    this.map = value;
                    if (this.map != null)
                    {
                        this.map.Updated += this.Map_Updated;
                    }
                    this.Invalidate();
                }
            }
        }
        private Map map;

        private int MapId
        {
            get { return this.ViewModel.MapCollection.GetId(this.Map); }
        }

        private EditorState EditorState
        {
            get
            {
                if (this.ViewModel != null)
                {
                    return this.ViewModel.EditorState;
                }
                else
                {
                    return null;
                }
            }
        }

        private void ViewModel_Opened(object sender, EventArgs e)
        {
            this.SelectedMapIdChanged();
            this.MapEditorModeChanged();
        }

        private void EditorState_Updated(object sender, UpdatedEventArgs e)
        {
            switch (e.PropertyName)
            {
            case "SelectedMapId":
                this.SelectedMapIdChanged();
                break;
            case "MapEditorMode":
                this.MapEditorModeChanged();
                break;
            case "MapOffsets":
                if ((int)e.Tag == this.ViewModel.MapCollection.GetId(this.Map))
                {
                    this.Invalidate();
                }
                break;
            default:
                throw new ArgumentException("Invalid property name", "e");
            }
        }

        private void SelectedMapIdChanged()
        {
            this.Map = this.EditorState.SelectedMap;
        }

        private void MapEditorModeChanged()
        {
            switch (this.EditorState.MapEditorMode)
            {
            case MapEditorMode.Scroll:
                this.Cursor = Cursors.Hand;
                break;
            default:
                this.Cursor = Cursors.Default;
                break;
            }
        }

        private void Map_Updated(object sender, UpdatedEventArgs e)
        {
            switch (e.PropertyName)
            {
            case "Width":
            case "Height":
                this.Invalidate();
                break;
            }
        }

        private bool IsMoving = false;
        private Point StartMousePoint;
        private Point StartOffset;

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            if (this.EditorState.MapEditorMode == MapEditorMode.Scroll &&
                (e.Button & MouseButtons.Left) != 0)
            {
                this.StartMousePoint = e.Location;
                this.StartOffset = this.EditorState.GetMapOffset(this.MapId);
                this.IsMoving = true;
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (this.IsMoving && (e.Button & MouseButtons.Left) != 0)
            {
                Debug.Assert(this.EditorState.MapEditorMode == MapEditorMode.Scroll);
                int id = this.MapId;
                Point point = this.EditorState.GetMapOffset(id);
                point.X = this.StartOffset.X + (e.X - this.StartMousePoint.X);
                point.Y = this.StartOffset.Y + (e.Y - this.StartMousePoint.Y);
                this.EditorState.SetMapOffset(id, point);
            }
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            try
            {
                if (this.IsMoving && (e.Button & MouseButtons.Left) != 0)
                {
                    Debug.Assert(this.EditorState.MapEditorMode == MapEditorMode.Scroll);
                    int id = this.MapId;
                    Point point = this.EditorState.GetMapOffset(id);
                    point.X = this.StartOffset.X + (e.X - this.StartMousePoint.X);
                    point.Y = this.StartOffset.Y + (e.Y - this.StartMousePoint.Y);
                    this.EditorState.SetMapOffset(id, point);
                }
            }
            finally
            {
                this.IsMoving = false;
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            Graphics g = e.Graphics;
            if (this.ViewModel != null && this.EditorState != null)
            {
                Map map = this.Map;
                if (map != null)
                {
                    Point position = this.EditorState.GetMapOffset(this.MapId);
                    int width = map.Width;
                    int height = map.Height;
                    for (int j = 0; j < height; j++)
                    {
                        int y = j * 32 + position.Y;
                        if (y + 32 <= 0)
                        {
                            continue;
                        }
                        else if (this.ClientSize.Height <= y)
                        {
                            break;
                        }
                        for (int i = 0; i < width; i++)
                        {
                            int x = i * 32 + position.X;
                            if (x + 32 <= 0)
                            {
                                continue;
                            }
                            else if (this.ClientSize.Width <= x)
                            {
                                break;
                            }
                            g.DrawImage(Util.BackgroundBitmap, x, y);
                        }
                    }
                }
            }
        }
    }
}
