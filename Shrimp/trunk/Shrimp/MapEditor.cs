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
            this.HScrollBar.SmallChange = Util.BackgroundGridSize;
            this.HScrollBar.LargeChange = Math.Max(this.HScrollBar.Width, 0);
            this.VScrollBar.SmallChange = Util.BackgroundGridSize;
            this.VScrollBar.LargeChange = Math.Max(this.VScrollBar.Height, 0);
            if (this.Map != null)
            {
                Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                int hMax = this.Map.Width * Util.BackgroundGridSize - this.HScrollBar.Width; // TODO
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
                int vMax = this.Map.Height * Util.BackgroundGridSize - this.VScrollBar.Height; // TODO
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
                Math.Min(Math.Max(mousePosition.X / Util.BackgroundGridSize, 0), this.Map.Width - 1);
            this.CursorTileY =
                Math.Min(Math.Max(mousePosition.Y / Util.BackgroundGridSize, 0), this.Map.Height - 1);
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
                    Math.Min(Math.Max(mousePosition.X / Util.BackgroundGridSize, 0), this.Map.Width - 1);
                this.CursorTileY =
                    Math.Min(Math.Max(mousePosition.Y / Util.BackgroundGridSize, 0), this.Map.Height - 1);
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

        private int GridSize
        {
            get
            {
                // TODO
                return Util.BackgroundGridSize;
            }
        }

        private Size OffscreenSize
        {
            get
            {
                return new Size
                {
                    Width = this.HScrollBar.Width,
                    Height = this.VScrollBar.Height,
                };
            }
        }
        private IntPtr HOffscreen = IntPtr.Zero;
        private IntPtr HOffscreenDC = IntPtr.Zero;
        private unsafe IntPtr OffscreenPixels = IntPtr.Zero;

        protected override void OnLayout(LayoutEventArgs e)
        {
            base.OnLayout(e);
            this.AdjustScrollBars();
            if (this.HOffscreen != IntPtr.Zero)
            {
                Win32API.DeleteDC(this.HOffscreenDC);
                Win32API.DeleteObject(this.HOffscreen);
                this.OffscreenPixels = IntPtr.Zero;
                this.HOffscreenDC = IntPtr.Zero;
                this.HOffscreen = IntPtr.Zero;
            }
            Win32API.BITMAPINFO bitmapInfo = new Win32API.BITMAPINFO
            {
                bmiHeader = new Win32API.BITMAPINFOHEADER
                {
                    biSize = (uint)Marshal.SizeOf(typeof(Win32API.BITMAPINFOHEADER)),
                    biWidth = this.OffscreenSize.Width,
                    biHeight = -this.OffscreenSize.Height,
                    biPlanes = 1,
                    biBitCount = 32,
                    biCompression = Win32API.BI_RGB,
                },
            };
            IntPtr hDC = IntPtr.Zero;
            try
            {
                hDC = Win32API.GetDC(this.Handle);
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
                Size = this.OffscreenSize,
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
            Debug.Assert(this.HOffscreenDC != IntPtr.Zero);
            Point offset = this.EditorState.GetMapOffset(this.Map.Id);
            int offscreenWidth = this.OffscreenSize.Width;
            int offscreenHeight = this.OffscreenSize.Height;
            Size offscreenSize = new Size(offscreenWidth, offscreenHeight);
            int bgGridSize = Util.BackgroundGridSize;
            int bgStartI = Math.Max(-offset.X / bgGridSize, 0);
            int bgEndI = Math.Min((offscreenWidth - offset.X) / bgGridSize + 1, map.Width);
            int bgStartJ = Math.Max(-offset.Y / bgGridSize, 0);
            int bgEndJ = Math.Min((offscreenHeight - offset.Y) / bgGridSize + 1, map.Height);
            
            // adjust
            int tmpBgStartX = bgStartI * bgGridSize + offset.X;
            int tmpBgEndX = bgEndI * bgGridSize + offset.X;
            int tmpBgStartY = bgStartJ * bgGridSize + offset.Y;
            int tmpBgEndY = bgEndJ * bgGridSize + offset.Y;
            int paddingI1 = (rect.Left - tmpBgStartX) / bgGridSize;
            int paddingI2 = (tmpBgEndX - rect.Right) / bgGridSize;
            int paddingJ1 = (rect.Top - tmpBgStartY) / bgGridSize;
            int paddingJ2 = (tmpBgEndY - rect.Bottom) / bgGridSize;
            bgStartI += paddingI1;
            bgEndI -= paddingI2;
            bgStartJ += paddingJ1;
            bgEndJ -= paddingJ2;

            int tileGridSize = this.GridSize;
            int tileStartI = Math.Max(-offset.X / tileGridSize, 0);
            int tileEndI = Math.Min((offscreenWidth - offset.X) / tileGridSize + 1, map.Width);
            int tileStartJ = Math.Max(-offset.Y / tileGridSize, 0);
            int tileEndJ = Math.Min((offscreenHeight - offset.Y) / tileGridSize + 1, map.Height);
            Debug.Assert(tileGridSize <= bgGridSize);
            Debug.Assert(bgGridSize % tileGridSize == 0);
            tileStartI += paddingI1 * bgGridSize / tileGridSize;
            tileEndJ -= paddingI2 * bgGridSize / tileGridSize;
            tileStartJ += paddingJ1 * bgGridSize / tileGridSize;
            tileEndJ -= paddingJ2 * bgGridSize / tileGridSize;

            rect.X = bgStartI * bgGridSize + offset.X;
            rect.Width = (bgEndI - bgStartI) * bgGridSize;
            rect.Y = bgStartJ * bgGridSize + offset.Y;
            rect.Height = (bgEndJ - bgStartJ) * bgGridSize;

            using (Graphics g = Graphics.FromHdc(this.HOffscreenDC))
            {
                g.FillRectangle(SystemBrushes.Control, rect);
                BitmapData bd = null;
                try
                {
                    bd = Util.BackgroundBitmap.LockBits(new Rectangle
                    {
                        Size = Util.BackgroundBitmap.Size,
                    }, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                    for (int j = bgStartJ; j < bgEndJ; j++)
                    {
                        int y = j * bgGridSize + offset.Y;
                        for (int i = bgStartI; i < bgEndI; i++)
                        {
                            int x = i * bgGridSize + offset.X;
                            Win32API.BitBlt(
                                this.HOffscreenDC, x, y, bgGridSize, bgGridSize,
                                Util.HBackgroundBitmapDC, 0, 0,
                                Win32API.TernaryRasterOperations.SRCCOPY);
                        }
                    }
                }
                finally
                {
                    if (bd != null)
                    {
                        Util.BackgroundBitmap.UnlockBits(bd);
                        bd = null;
                    }
                }
                Dictionary<Bitmap, BitmapData> bdHash = null;
                try
                {
                    TileSetCollection tileSetCollection = this.ViewModel.TileSetCollection;
                    bdHash = new Dictionary<Bitmap, BitmapData>();
                    LayerMode layerMode = this.EditorState.LayerMode;
                    Pen eventGridPen = new Pen(Color.FromArgb(0x40, 0x80, 0x80, 0x80));
                    for (int layer = 0; layer < 2; layer++)
                    {
                        byte alpha = 255;
                        if (layerMode == LayerMode.Layer1 && layer == 1)
                        {
                            alpha = 128;
                        }
                        for (int j = tileStartJ; j < tileEndJ; j++)
                        {
                            int y = j * tileGridSize + offset.Y;
                            for (int i = tileStartI; i < tileEndI; i++)
                            {
                                int x = i * tileGridSize + offset.X;
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
                                    int srcX = (tileId % Util.PaletteHorizontalCount) * tileGridSize;
                                    int srcY = (tileId / Util.PaletteHorizontalCount) * tileGridSize;
                                    Util.DrawBitmap(this.OffscreenPixels, offscreenSize,
                                        x, y, tileGridSize, tileGridSize, srcBD, srcX, srcY, alpha);
                                }
                                if (layerMode == LayerMode.Event)
                                {
                                    g.DrawRectangle(eventGridPen, new Rectangle
                                    {
                                        X = x,
                                        Y = y,
                                        Width = tileGridSize - 1,
                                        Height = tileGridSize - 1
                                    });
                                }
                            }
                        }
                        if (this.EditorState.LayerMode == LayerMode.Layer2 && layer == 0)
                        {
                            this.DarkenOffscreen(offscreenSize, rect);
                        }
                    }
                }
                finally
                {
                    if (bdHash != null)
                    {
                        foreach (var pair in bdHash)
                        {
                            pair.Key.UnlockBits(pair.Value);
                        }
                        bdHash.Clear();
                        bdHash = null;
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
                int gridSize = this.GridSize;
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
