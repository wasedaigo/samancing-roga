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
            this.SuspendLayout();
            this.VScrollBar.Location = new Point
            {
                X = this.ClientSize.Width - this.VScrollBar.Width,
                Y = 0,
            };
            this.VScrollBar.Height = this.ClientSize.Height - this.HScrollBar.Height;
            this.HScrollBar.Location = new Point
            {
                X = 0,
                Y = this.ClientSize.Height - this.HScrollBar.Height,
            };
            this.HScrollBar.Width = this.ClientSize.Width - this.VScrollBar.Width;
            this.ResumeLayout(false);
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
                        this.viewModel.IsOpenedChanged -= this.ViewModel_IsOpenedChanged;
                        this.viewModel.EditorState.Updated -= this.EditorState_Updated;
                    }
                    this.viewModel = value;
                    if (this.viewModel != null)
                    {
                        this.Map = this.viewModel.EditorState.SelectedMap;
                        this.viewModel.IsOpenedChanged += this.ViewModel_IsOpenedChanged;
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

        private void ViewModel_IsOpenedChanged(object sender, EventArgs e)
        {
            if (this.ViewModel.IsOpened)
            {
                this.SelectedMapIdChanged();
            }
        }

        private void EditorState_Updated(object sender, UpdatedEventArgs e)
        {
            switch (e.PropertyName)
            {
            case "SelectedMapId":
                this.SelectedMapIdChanged();
                break;
            case "LayerMode":
                this.Invalidate();
                break;
            case "MapOffsets":
                if (this.Map != null && (int)e.Tag == this.Map.Id)
                {
                    this.AdjustScrollBars();
                    this.Invalidate();
                }
                break;
            }
        }

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
                    this.AdjustScrollBars();
                    this.Invalidate();
                }
            }
        }
        private Map map;

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

        private void SelectedMapIdChanged()
        {
            this.Map = this.EditorState.SelectedMap;
        }

        private void Map_Updated(object sender, UpdatedEventArgs e)
        {
            switch (e.PropertyName)
            {
            case "Width":
            case "Height":
                this.AdjustScrollBars();
                this.Invalidate();
                break;
            case "Tiles":
                this.Invalidate();
                break;
            }
        }

        private void AdjustScrollBars()
        {
            this.HScrollBar.SmallChange = 32;
            this.HScrollBar.LargeChange = Math.Max(this.HScrollBar.Width, 0);
            this.VScrollBar.SmallChange = 32;
            this.VScrollBar.LargeChange = Math.Max(this.VScrollBar.Height, 0);
            if (this.Map != null)
            {
                Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                int hMax = this.Map.Width * 32 - this.HScrollBar.Width;
                if (0 < hMax)
                {
                    this.HScrollBar.Enabled = true;
                    this.HScrollBar.Minimum = 0;
                    this.HScrollBar.Maximum = hMax + this.HScrollBar.LargeChange;
                    this.HScrollBar.Value = Math.Min(Math.Max(0, -offset.X), hMax);
                }
                else
                {
                    this.HScrollBar.Enabled = false;
                    this.HScrollBar.Value = 0;
                }
                int vMax = this.Map.Height * 32 - this.VScrollBar.Height;
                if (0 < vMax)
                {
                    this.VScrollBar.Enabled = true;
                    this.VScrollBar.Minimum = 0;
                    this.VScrollBar.Maximum = vMax + this.VScrollBar.LargeChange;
                    this.VScrollBar.Value = Math.Min(Math.Max(0, -offset.Y), vMax);
                }
                else
                {
                    this.VScrollBar.Enabled = false;
                    this.VScrollBar.Value = 0;
                }
                this.EditorState.SetMapOffset(this.Map.Id, new Point
                {
                    X = -this.HScrollBar.Value,
                    Y = -this.VScrollBar.Value,
                });
            }
            else
            {
                this.HScrollBar.Enabled = false;
                this.VScrollBar.Enabled = false;
                this.HScrollBar.Value = 0;
                this.VScrollBar.Value = 0;
            }
        }

        /*protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyDown(e);
            Point offset = this.EditorState.GetMapOffset(this.Map.Id);
            if ((e.KeyCode & Keys.Up) != 0)
            {
                this.EditorState.SetMapOffset(this.Map.Id, new Point
                {
                    X = offset.X,
                    Y = offset.Y - Util.DisplayedGridSize,
                });
            }
            else if ((e.KeyCode & Keys.Down) != 0)
            {
                this.EditorState.SetMapOffset(this.Map.Id, new Point
                {
                    X = offset.X,
                    Y = offset.Y + Util.DisplayedGridSize,
                });
            }
            else if ((e.KeyCode & Keys.Left) != 0)
            {
                this.EditorState.SetMapOffset(this.Map.Id, new Point
                {
                    X = offset.X - Util.DisplayedGridSize,
                    Y = offset.Y,
                });
            }
            else if ((e.KeyCode & Keys.Right) != 0)
            {
                this.EditorState.SetMapOffset(this.Map.Id, new Point
                {
                    X = offset.X + Util.DisplayedGridSize,
                    Y = offset.Y,
                });
            }
        }*/

        private int CursorTileX = 0;
        private int CursorTileY = 0;

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            if (this.EditorState.LayerMode != LayerMode.Event)
            {
                if ((e.Button & MouseButtons.Left) != 0)
                {
                    int layer = 0;
                    switch (this.EditorState.LayerMode)
                    {
                    case LayerMode.Layer1: layer = 0; break;
                    case LayerMode.Layer2: layer = 1; break;
                    default: Debug.Fail("Invalid layer"); break;
                    }
                    this.Map.SetTiles(layer, this.CursorTileX, this.CursorTileY, this.EditorState.SelectedTiles);
                }
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (this.EditorState.LayerMode != LayerMode.Event)
            {
                Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                Point mousePosition = new Point
                {
                    X = e.X - offset.X,
                    Y = e.Y - offset.Y,
                };
                this.CursorTileX =
                    Math.Min(Math.Max(mousePosition.X / Util.DisplayedGridSize, 0), this.Map.Width - 1);
                this.CursorTileY =
                    Math.Min(Math.Max(mousePosition.Y / Util.DisplayedGridSize, 0), this.Map.Height - 1);
                if ((e.Button & MouseButtons.Left) != 0)
                {
                    int layer = 0;
                    switch (this.EditorState.LayerMode)
                    {
                    case LayerMode.Layer1: layer = 0; break;
                    case LayerMode.Layer2: layer = 1; break;
                    default: Debug.Fail("Invalid layer"); break;
                    }
                    this.Map.SetTiles(layer, this.CursorTileX, this.CursorTileY, this.EditorState.SelectedTiles);
                }
                else
                {
                    this.Invalidate();
                }
            }
        }

        protected override void OnLayout(LayoutEventArgs e)
        {
            base.OnLayout(e);
            this.AdjustScrollBars();
            this.Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            Graphics g = e.Graphics;
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            if (this.ViewModel != null && this.EditorState != null)
            {
                Map map = this.Map;
                if (map != null)
                {
                    Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                    int width = map.Width;
                    int height = map.Height;
                    Pen gridPen = new Pen(Color.FromArgb(0x80, 0x80, 0x80, 0x80), 1);
                    TileSetCollection tileSetCollection = this.ViewModel.TileSetCollection;
                    Dictionary<int, Bitmap> bitmaps = new Dictionary<int, Bitmap>();
                    for (int j = 0; j < height; j++)
                    {
                        int y = j * 32 + offset.Y;
                        if (y + 32 <= 0)
                        {
                            continue;
                        }
                        else if (this.VScrollBar.Height <= y)
                        {
                            break;
                        }
                        for (int i = 0; i < width; i++)
                        {
                            int x = i * 32 + offset.X;
                            if (x + 32 <= 0)
                            {
                                continue;
                            }
                            else if (this.HScrollBar.Width <= x)
                            {
                                break;
                            }
                            g.DrawImage(Util.BackgroundBitmap, x, y);
                            Tile tile = map.GetTile(0, i, j);
                            int tileId = tile.TileId;
                            if (tileSetCollection.ContainsId(tile.TileSetId))
                            {
                                int scale = 2;
                                if (!bitmaps.ContainsKey(tile.TileSetId))
                                {
                                    TileSet tileSet = tileSetCollection.GetItem(tile.TileSetId);
                                    bitmaps[tile.TileSetId]= tileSet.GetBitmap(BitmapScale.Scale1);
                                }
                                g.DrawImage(bitmaps[tile.TileSetId], x, y, new Rectangle
                                {
                                    X = (tileId % Util.PaletteHorizontalCount) * Util.GridSize * scale,
                                    Y = (tileId / Util.PaletteHorizontalCount) * Util.GridSize * scale,
                                    Width = 32,
                                    Height = 32,
                                }, GraphicsUnit.Pixel);
                            }
                            if (this.EditorState.LayerMode == LayerMode.Event)
                            {
                                g.DrawRectangle(gridPen, x, y, 32 - 1, 32 - 1);
                            }
                        }
                    }
                    SelectedTiles selectedTiles = this.EditorState.SelectedTiles;
                    Util.DrawFrame(g, new Rectangle
                    {
                        X = this.CursorTileX * 32 + offset.X,
                        Y = this.CursorTileY * 32 + offset.Y,
                        Width = 32 * selectedTiles.Width,
                        Height = 32 * selectedTiles.Height,
                    });
                }
            }
            g.FillRectangle(new SolidBrush(this.BackColor), new Rectangle
            {
                X = this.VScrollBar.Location.X,
                Y = this.HScrollBar.Location.Y,
                Width = this.VScrollBar.Width,
                Height = this.HScrollBar.Height,
            });
        }

        private void ScrollBar_Scroll(object sender, ScrollEventArgs e)
        {
            this.EditorState.SetMapOffset(this.Map.Id, new Point
            {
                X = -this.HScrollBar.Value,
                Y = -this.VScrollBar.Value,
            });
        }
    }
}
