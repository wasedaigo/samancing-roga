using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
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
                this.UpdateOffscreen();
                this.Invalidate();
                this.Update();
                break;
            case "MapOffsets":
                if (this.Map != null && (int)e.Tag == this.Map.Id)
                {
                    this.AdjustScrollBars();
                    this.UpdateOffscreen();
                    this.Invalidate();
                    this.Update();
                }
                break;
            case "SelectedTiles":
                if (this.EditorState.SelectedTiles.SelectedTilesType != SelectedTilesType.Picker)
                {
                    this.CursorOffsetX = 0;
                    this.CursorOffsetY = 0;
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
                    this.UpdateOffscreen();
                    this.Invalidate();
                    this.Update();
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
                this.UpdateOffscreen();
                this.Invalidate();
                this.Update();
                break;
            case "Tiles":
                this.UpdateOffscreen();
                this.Invalidate();
                this.Update();
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

        private int CursorTileX = 0;
        private int CursorTileY = 0;
        private int CursorOffsetX = 0;
        private int CursorOffsetY = 0;
        private bool IsPickingTiles = false;
        private int PickerStartX = 0;
        private int PickerStartY = 0;
        private int RenderingStartX = 0;
        private int RenderingStartY = 0;

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            Point offset = this.EditorState.GetMapOffset(this.Map.Id);
            Point mousePosition = new Point
            {
                X = e.X - offset.X,
                Y = e.Y - offset.Y,
            };
            Rectangle oldFrameRect = this.FrameRect;
            this.CursorTileX =
                Math.Min(Math.Max(mousePosition.X / Util.DisplayedGridSize, 0), this.Map.Width - 1);
            this.CursorTileY =
                Math.Min(Math.Max(mousePosition.Y / Util.DisplayedGridSize, 0), this.Map.Height - 1);
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
                    int x = this.CursorTileX + this.CursorOffsetX;
                    int y = this.CursorTileY + this.CursorOffsetY;
                    this.RenderingStartX = x;
                    this.RenderingStartY = y;
                    this.Map.SetTiles(layer, x, y, this.EditorState.SelectedTiles, 0, 0);
                }
                else if ((e.Button & MouseButtons.Right) != 0)
                {
                    this.CursorOffsetX = 0;
                    this.CursorOffsetY = 0;
                    this.PickerStartX = this.CursorTileX;
                    this.PickerStartY = this.CursorTileY;
                    this.IsPickingTiles = true;
                    this.Invalidate(oldFrameRect);
                    this.Update();
                }
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (this.EditorState.LayerMode != LayerMode.Event)
            {
                Rectangle oldFrameRect = this.FrameRect;
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
                if (this.IsPickingTiles)
                {
                    if ((e.Button & MouseButtons.Right) != 0)
                    {
                        this.Invalidate(oldFrameRect);
                        this.Update();
                    }
                }
                else
                {
                    SelectedTiles selectedTiles = this.EditorState.SelectedTiles;
                    if ((e.Button & MouseButtons.Left) != 0)
                    {
                        int layer = 0;
                        switch (this.EditorState.LayerMode)
                        {
                        case LayerMode.Layer1: layer = 0; break;
                        case LayerMode.Layer2: layer = 1; break;
                        default: Debug.Fail("Invalid layer"); break;
                        }
                        int x = this.CursorTileX + this.CursorOffsetX;
                        int y = this.CursorTileY + this.CursorOffsetY;
                        if (!this.Map.SetTiles(layer, x, y, selectedTiles,
                            x - this.RenderingStartX, y - this.RenderingStartY))
                        {
                            this.Invalidate(oldFrameRect);
                            this.Update();
                        }
                    }
                    else
                    {
                        this.Invalidate(oldFrameRect);
                        this.Update();
                    }
                }
            }
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            if (this.IsPickingTiles)
            {
                if ((e.Button & MouseButtons.Right) != 0)
                {
                    int pickedRegionX = Math.Min(this.CursorTileX, this.PickerStartX);
                    int pickedRegionY = Math.Min(this.CursorTileY, this.PickerStartY);
                    int pickedRegionWidth = Math.Abs(this.CursorTileX - this.PickerStartX) + 1;
                    int pickedRegionHeight = Math.Abs(this.CursorTileY - this.PickerStartY) + 1;
                    this.CursorOffsetX = pickedRegionX - this.CursorTileX;
                    this.CursorOffsetY = pickedRegionY - this.CursorTileY;
                    int layer = 0;
                    switch (this.EditorState.LayerMode)
                    {
                    case LayerMode.Layer1: layer = 0; break;
                    case LayerMode.Layer2: layer = 1; break;
                    default: Debug.Fail("Invalid layer"); break;
                    }
                    Map map = this.Map;
                    List<Tile> tiles = new List<Tile>();
                    for (int j = 0; j < pickedRegionHeight; j++)
                    {
                        for (int i = 0; i < pickedRegionWidth; i++)
                        {
                            tiles.Add(map.GetTile(layer, i + pickedRegionX, j + pickedRegionY));
                        }
                    }
                    if (tiles.Count == 1)
                    {
                        this.EditorState.SelectedTiles = SelectedTiles.Single(tiles[0]);
                    }
                    else
                    {
                        this.EditorState.SelectedTiles =
                            SelectedTiles.Picker(tiles, pickedRegionWidth, pickedRegionHeight);
                    }
                }
                this.IsPickingTiles = false;
            }
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            base.OnMouseLeave(e);
            this.Invalidate(this.FrameRect);
            this.Update();
        }

        private IntPtr HOffscreen = IntPtr.Zero;
        private IntPtr HOffscreenDC = IntPtr.Zero;

        protected override void OnLayout(LayoutEventArgs e)
        {
            base.OnLayout(e);
            this.AdjustScrollBars();
            if (this.HOffscreen == IntPtr.Zero)
            {
                Win32API.DeleteDC(this.HOffscreenDC);
                Win32API.DeleteObject(this.HOffscreen);
                this.HOffscreenDC = IntPtr.Zero;
                this.HOffscreen = IntPtr.Zero;
            }
            int width = this.HScrollBar.Width;
            int height = this.VScrollBar.Height;
            IntPtr hDC = Win32API.GetDC(this.Handle);
            this.HOffscreen = Win32API.CreateCompatibleBitmap(hDC, width, height);
            this.HOffscreenDC = Win32API.CreateCompatibleDC(hDC);
            Win32API.SelectObject(this.HOffscreenDC, this.HOffscreen);
            Win32API.ReleaseDC(this.Handle, hDC);
            this.UpdateOffscreen();
            this.Invalidate();
            this.Update();
        }

        private void UpdateOffscreen()
        {
            this.UpdateOffscreen(Rectangle.Empty);
        }

        private void UpdateOffscreen(Rectangle rectangle)
        {
            if (this.ViewModel != null && this.EditorState != null)
            {
                Map map = this.Map;
                if (map != null)
                {
                    Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                    int width = map.Width;
                    int height = map.Height;
                    using (Graphics g = Graphics.FromHdc(this.HOffscreenDC))
                    {
                        g.Clear(SystemColors.Control);
                        {
                            int startI = Math.Max(-offset.X / Util.DisplayedGridSize, 0);
                            int endI = Math.Min((this.HScrollBar.Width - offset.X) / Util.DisplayedGridSize + 1, width);
                            int startJ = Math.Max(-offset.Y / Util.DisplayedGridSize, 0);
                            int endJ = Math.Min((this.VScrollBar.Height - offset.Y) / Util.DisplayedGridSize + 1, height);
                            for (int j = startJ; j < endJ; j++)
                            {
                                int y = j * Util.DisplayedGridSize + offset.Y;
                                for (int i = startI; i < endI; i++)
                                {
                                    int x = i * Util.DisplayedGridSize + offset.X;
                                    Win32API.BitBlt(
                                        this.HOffscreenDC, x, y, 32, 32,
                                        Util.HBackgroundBitmapDC, 0, 0,
                                        Win32API.TernaryRasterOperations.SRCCOPY);
                                }
                            }
                        }
                        int gridSize = Util.GridSize * 2;
                        {
                            TileSetCollection tileSetCollection = this.ViewModel.TileSetCollection;
                            Dictionary<int, IntPtr> dcHash = new Dictionary<int, IntPtr>();
                            for (int layer = 0; layer < 2; layer++)
                            {
                                byte alpha = 255;
                                if (this.EditorState.LayerMode == LayerMode.Layer1 && layer == 1)
                                {
                                    alpha = 128;
                                }
                                Win32API.BLENDFUNCTION blend = new Win32API.BLENDFUNCTION
                                {
                                    BlendOp = Win32API.AC_SRC_OVER,
                                    BlendFlags = 0,
                                    SourceConstantAlpha = alpha,
                                    AlphaFormat = Win32API.AC_SRC_ALPHA
                                };
                                int startI = Math.Max(-offset.X / gridSize, 0);
                                int endI = Math.Min((this.HScrollBar.Width - offset.X) / gridSize + 1, width);
                                int startJ = Math.Max(-offset.Y / gridSize, 0);
                                int endJ = Math.Min((this.VScrollBar.Height - offset.Y) / gridSize + 1, height);
                                for (int j = startJ; j < endJ; j++)
                                {
                                    int y = j * gridSize + offset.Y;
                                    for (int i = startI; i < endI; i++)
                                    {
                                        int x = i * gridSize + offset.X;
                                        Tile tile = map.GetTile(layer, i, j);
                                        if (tileSetCollection.ContainsId(tile.TileSetId))
                                        {
                                            int tileId = tile.TileId;
                                            IntPtr hSrcDC;
                                            if (!dcHash.ContainsKey(tile.TileSetId))
                                            {
                                                TileSet tileSet = tileSetCollection.GetItem(tile.TileSetId);
                                                hSrcDC = dcHash[tile.TileSetId] = tileSet.GetHBitmap(BitmapScale.Scale1);
                                            }
                                            else
                                            {
                                                hSrcDC = dcHash[tile.TileSetId];
                                            }
                                            int srcX = (tileId % Util.PaletteHorizontalCount) * gridSize;
                                            int srcY = (tileId / Util.PaletteHorizontalCount) * gridSize;
                                            Win32API.AlphaBlend(
                                                this.HOffscreenDC, x, y, gridSize, gridSize,
                                                hSrcDC, srcX, srcY, gridSize, gridSize,
                                                blend);
                                            /*g.DrawImage(tileSet.GetBitmap(BitmapScale.Scale1),
                                                x, y,
                                                new Rectangle(srcX, srcY, gridSize, gridSize),
                                                GraphicsUnit.Pixel);*/
                                        }
                                    }
                                }
                                if (this.EditorState.LayerMode == LayerMode.Layer2 && layer == 0)
                                {
                                    //Util.DarkenBitmap(dstBD);
                                }
                            }
                        }
                        //this.OffscreenBitmap.UnlockBits(dstBD);
                    }
                }
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            Graphics g = e.Graphics;
            if (this.ViewModel != null && this.EditorState != null && this.Map != null)
            {
                Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                Rectangle rect = e.ClipRectangle;
                /*using (Graphics gOffscreen = Graphics.FromImage(this.OffscreenBitmap))
                {
                    IntPtr hDstDC = g.GetHdc();
                    IntPtr hOffscreenDC = this.OffscreenBitmap.GetHbitmap();
                    IntPtr hSrcDC = gOffscreen.GetHdc();
                    IntPtr hOffscreenDC2 = Win32API.SelectObject(hSrcDC, hOffscreenDC);
                    Win32API.BitBlt(
                        hDstDC, rect.X, rect.Y, rect.Width, rect.Height,
                        hSrcDC, rect.X, rect.Y, Win32API.TernaryRasterOperations.SRCCOPY);
                    Win32API.SelectObject(hSrcDC, hOffscreenDC2);
                    gOffscreen.ReleaseHdc(hSrcDC);
                    Win32API.DeleteObject(hOffscreenDC);
                    g.ReleaseHdc(hDstDC);
                }*/
                IntPtr hDstDC = g.GetHdc();
                Win32API.BitBlt(
                    hDstDC, rect.X, rect.Y, rect.Width, rect.Height,
                    this.HOffscreenDC, rect.X, rect.Y,
                    Win32API.TernaryRasterOperations.SRCCOPY);
                g.ReleaseHdc(hDstDC);
                Util.DrawFrame(g, this.FrameRect);
            }
            g.FillRectangle(new SolidBrush(this.BackColor), new Rectangle
            {
                X = this.VScrollBar.Location.X,
                Y = this.HScrollBar.Location.Y,
                Width = this.VScrollBar.Width,
                Height = this.HScrollBar.Height,
            });
        }

        private Rectangle FrameRect
        {
            get
            {
                Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                int gridSize = Util.GridSize * 2;
                if (!this.IsPickingTiles)
                {
                    SelectedTiles selectedTiles = this.EditorState.SelectedTiles;
                    return new Rectangle
                    {
                        X = (this.CursorTileX + this.CursorOffsetX) * gridSize + offset.X,
                        Y = (this.CursorTileY + this.CursorOffsetY) * gridSize + offset.Y,
                        Width = gridSize * selectedTiles.Width,
                        Height = gridSize * selectedTiles.Height,
                    };
                }
                else
                {
                    int pickedRegionX = Math.Min(this.CursorTileX, this.PickerStartX);
                    int pickedRegionY = Math.Min(this.CursorTileY, this.PickerStartY);
                    int pickedRegionWidth = Math.Abs(this.CursorTileX - this.PickerStartX) + 1;
                    int pickedRegionHeight = Math.Abs(this.CursorTileY - this.PickerStartY) + 1;
                    return new Rectangle
                    {
                        X = pickedRegionX * gridSize + offset.X,
                        Y = pickedRegionY * gridSize + offset.Y,
                        Width = gridSize * pickedRegionWidth,
                        Height = gridSize * pickedRegionHeight,
                    };
                }
            }
        }

        private void HScrollBar_Scroll(object sender, ScrollEventArgs e)
        {
            Point offset = this.EditorState.GetMapOffset(this.Map.Id);
            this.EditorState.SetMapOffset(this.Map.Id, new Point
            {
                X = -e.NewValue,
                Y = offset.Y,
            });
        }

        private void VScrollBar_Scroll(object sender, ScrollEventArgs e)
        {
            Point offset = this.EditorState.GetMapOffset(this.Map.Id);
            this.EditorState.SetMapOffset(this.Map.Id, new Point
            {
                X = offset.X,
                Y = -e.NewValue,
            });
        }
    }
}
