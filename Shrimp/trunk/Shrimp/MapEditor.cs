using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Data;
using System.Linq;
using System.Runtime.InteropServices;
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
                /*this.UpdateOffscreen(this.FrameRect);
                this.Invalidate(this.FrameRect);*/
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
            else
            {
                this.Invalidate();
                this.Update();
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
                        if (this.EditorState.LayerMode != LayerMode.Event)
                        {
                            this.Invalidate(oldFrameRect);
                        }
                        else
                        {
                            this.Invalidate();
                        }
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
        private unsafe IntPtr OffscreenPixels = IntPtr.Zero;

        protected override void OnLayout(LayoutEventArgs e)
        {
            base.OnLayout(e);
            this.AdjustScrollBars();
            if (this.HOffscreen == IntPtr.Zero)
            {
                Win32API.DeleteDC(this.HOffscreenDC);
                Win32API.DeleteObject(this.HOffscreen);
                this.OffscreenPixels = IntPtr.Zero;
                this.HOffscreenDC = IntPtr.Zero;
                this.HOffscreen = IntPtr.Zero;
            }
            int width = this.HScrollBar.Width;
            int height = this.VScrollBar.Height;
            IntPtr hDC = IntPtr.Zero;
            try
            {
                hDC = Win32API.GetDC(this.Handle);
                //this.HOffscreen = Win32API.CreateCompatibleBitmap(hDC, width, height);
                Win32API.BITMAPINFO bitmapInfo = new Win32API.BITMAPINFO
                {
                    bmiHeader = new Win32API.BITMAPINFOHEADER
                    {
                        biSize = (uint)Marshal.SizeOf(typeof(Win32API.BITMAPINFOHEADER)),
                        biWidth = width,
                        biHeight = -height,
                        biPlanes = 1,
                        biBitCount = 32,
                        biCompression = Win32API.BI_RGB,
                    },
                };
                this.HOffscreen = Win32API.CreateDIBSection(hDC, ref bitmapInfo,
                    Win32API.DIB_RGB_COLORS, out this.OffscreenPixels, IntPtr.Zero, 0);
                this.HOffscreenDC = Win32API.CreateCompatibleDC(hDC);
                Win32API.SelectObject(this.HOffscreenDC, this.HOffscreen);
            }
            finally
            {
                if (hDC != IntPtr.Zero)
                {
                    Win32API.ReleaseDC(this.Handle, hDC);
                    hDC = IntPtr.Zero;
                }
            }
            this.UpdateOffscreen();
            this.Invalidate();
            this.Update();
        }

        private void UpdateOffscreen()
        {
            this.UpdateOffscreen(new Rectangle
            {
                X = 0,
                Y = 0,
                Width = this.HScrollBar.Width,
                Height = this.VScrollBar.Height,
            });
        }

        private void UpdateOffscreen(Rectangle rect)
        {
            if (this.ViewModel == null || this.EditorState == null)
            {
                return;
            }
            Map map = this.Map;
            if (map == null)
            {
                return;
            }
            Point offset = this.EditorState.GetMapOffset(this.Map.Id);
            int width = map.Width;
            int height = map.Height;
            int offscreenWidth = this.HScrollBar.Width;
            int offscreenHeight = this.VScrollBar.Height;
            Size offscreenSize = new Size(offscreenWidth, offscreenHeight);
            Debug.Assert(this.HOffscreenDC != IntPtr.Zero);
            using (Graphics g = Graphics.FromHdc(this.HOffscreenDC))
            {
                g.FillRectangle(SystemBrushes.Control, rect);
                {
                    int startI = Math.Max(-offset.X / Util.DisplayedGridSize, 0);
                    int endI = Math.Min((offscreenWidth - offset.X) / Util.DisplayedGridSize + 1, width);
                    int startJ = Math.Max(-offset.Y / Util.DisplayedGridSize, 0);
                    int endJ = Math.Min((offscreenHeight - offset.Y) / Util.DisplayedGridSize + 1, height);
                    BitmapData bd = Util.BackgroundBitmap.LockBits(new Rectangle
                    {
                        Size = Util.BackgroundBitmap.Size,
                    }, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                    for (int j = startJ; j < endJ; j++)
                    {
                        int y = j * Util.DisplayedGridSize + offset.Y;
                        for (int i = startI; i < endI; i++)
                        {
                            int x = i * Util.DisplayedGridSize + offset.X;
                            if (rect.Left <= x + 32 && x < rect.Right &&
                                rect.Top <= y + 32 && y < rect.Bottom)
                            {
                                Win32API.BitBlt(
                                    this.HOffscreenDC, x, y, 32, 32,
                                    Util.HBackgroundBitmapDC, 0, 0,
                                    Win32API.TernaryRasterOperations.SRCCOPY);
                            }
                        }
                    }
                    Util.BackgroundBitmap.UnlockBits(bd);
                }
                int gridSize = Util.GridSize * 2;
                {
                    TileSetCollection tileSetCollection = this.ViewModel.TileSetCollection;
                    Dictionary<Bitmap, BitmapData> bdHash = new Dictionary<Bitmap, BitmapData>();
                    LayerMode layerMode = this.EditorState.LayerMode;
                    Pen eventGridPen = new Pen(Color.FromArgb(0x40, 0x80, 0x80, 0x80));
                    for (int layer = 0; layer < 2; layer++)
                    {
                        byte alpha = 255;
                        if (layerMode == LayerMode.Layer1 && layer == 1)
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
                                if (rect.Left <= x + gridSize && x < rect.Right &&
                                    rect.Top <= y + gridSize && y < rect.Bottom)
                                {
                                    Tile tile = map.GetTile(layer, i, j);
                                    if (tileSetCollection.ContainsId(tile.TileSetId))
                                    {
                                        int tileId = tile.TileId;
                                        TileSet tileSet = tileSetCollection.GetItem(tile.TileSetId);
                                        Bitmap bitmap = tileSet.GetBitmap(BitmapScale.Scale1);
                                        BitmapData srcBD;
                                        if (!bdHash.ContainsKey(bitmap))
                                        {
                                            srcBD = bdHash[bitmap] = bitmap.LockBits(new Rectangle
                                            {
                                                Size = bitmap.Size,
                                            }, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                                        }
                                        else
                                        {
                                            srcBD = bdHash[bitmap];
                                        }
                                        int srcX = (tileId % Util.PaletteHorizontalCount) * gridSize;
                                        int srcY = (tileId / Util.PaletteHorizontalCount) * gridSize;
                                        Util.DrawBitmap(this.OffscreenPixels, offscreenSize,
                                            x, y, gridSize, gridSize, srcBD, srcX, srcY, alpha);
                                    }
                                    if (layerMode == LayerMode.Event)
                                    {
                                        g.DrawRectangle(eventGridPen, new Rectangle(x, y, gridSize - 1, gridSize - 1));
                                    }
                                }
                            }
                        }
                        if (this.EditorState.LayerMode == LayerMode.Layer2 && layer == 0)
                        {
                            this.DarkenOffscreen(offscreenSize, rect);
                        }
                    }
                    foreach (var pair in bdHash)
                    {
                        pair.Key.UnlockBits(pair.Value);
                    }
                }
            }
        }

        private void DarkenOffscreen(Size dstSize, Rectangle rect)
        {
            Debug.Assert(this.OffscreenPixels != IntPtr.Zero);
            int width = dstSize.Width;
            int height = dstSize.Height;
            int padding = (dstSize.Width * 4 + 3) / 4 * 4 - width * 4;
            int startI = Math.Max(rect.Left, 0);
            int startJ = Math.Max(rect.Top, 0);
            int endI = Math.Min(rect.Right, width);
            int endJ = Math.Min(rect.Bottom, height);
            unsafe
            {
                byte* dst = (byte*)this.OffscreenPixels;
                for (int j = startJ; j < endJ; j++, dst += padding)
                {
                    for (int i = startI; i < endI; i++, dst += 4)
                    {
                        dst[0] = (byte)(dst[0] >> 1);
                        dst[1] = (byte)(dst[1] >> 1);
                        dst[2] = (byte)(dst[2] >> 1);
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
                IntPtr hDstDC = IntPtr.Zero;
                try
                {
                    hDstDC = g.GetHdc();
                    Win32API.BitBlt(
                        hDstDC, rect.X, rect.Y, rect.Width, rect.Height,
                        this.HOffscreenDC, rect.X, rect.Y,
                        Win32API.TernaryRasterOperations.SRCCOPY);
                }
                finally
                {
                    if (hDstDC != IntPtr.Zero)
                    {
                        g.ReleaseHdc(hDstDC);
                        hDstDC = IntPtr.Zero;
                    }
                }
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
