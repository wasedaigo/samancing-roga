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
            this.SetStyle(ControlStyles.UserPaint, true);
            this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            this.HScrollBar.Width = this.ClientSize.Width - SystemInformation.VerticalScrollBarWidth;
            this.HScrollBar.Height = SystemInformation.HorizontalScrollBarHeight;
            this.HScrollBar.Left = 0;
            this.HScrollBar.Top = this.ClientSize.Height - SystemInformation.HorizontalScrollBarHeight;
            this.VScrollBar.Width = SystemInformation.VerticalScrollBarWidth;
            this.VScrollBar.Height = this.ClientSize.Height - SystemInformation.HorizontalScrollBarHeight;
            this.VScrollBar.Left = this.ClientSize.Width - SystemInformation.VerticalScrollBarWidth;
            this.VScrollBar.Top = 0;
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
                this.Update();
                break;
            case "MapOffsets":
                if (this.Map != null && e.ItemId == this.Map.Id)
                {
                    this.AdjustScrollBars();
                    Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                    Point previousOffset = (Point)e.PreviousValue;
                    int dx = offset.X - previousOffset.X;
                    int dy = offset.Y - previousOffset.Y;
                    NativeMethods.ScrollWindowEx(this.Handle, dx, dy,
                        IntPtr.Zero, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero, 0);
                    Rectangle rect = new Rectangle();
                    if (0 < dx)
                    {
                        rect.X = 0;
                        rect.Y = 0;
                        rect.Width = dx;
                        rect.Height = this.OffscreenSize.Height;
                        this.Invalidate(rect);
                    }
                    else if (dx < 0)
                    {
                        rect.X = this.OffscreenSize.Width + dx;
                        rect.Y = 0;
                        rect.Width = -dx;
                        rect.Height = this.OffscreenSize.Height;
                        this.Invalidate(rect);
                    }
                    if (0 < dy)
                    {
                        rect.X = 0;
                        rect.Y = 0;
                        rect.Width = this.OffscreenSize.Width;
                        rect.Height = dy;
                        this.Invalidate(rect);
                    }
                    else if (dy < 0)
                    {
                        rect.X = 0;
                        rect.Y = this.OffscreenSize.Height + dy;
                        rect.Width = this.OffscreenSize.Width;
                        rect.Height = -dy;
                        this.Invalidate(rect);
                    }
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
                this.Invalidate();
                this.Update();
                break;
            case "Tiles":
                Rectangle updatedTilesRect = (Rectangle)e.Bounds;
                Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                Rectangle updatedRect = new Rectangle
                {
                    X = updatedTilesRect.X * this.GridSize + offset.X,
                    Y = updatedTilesRect.Y * this.GridSize + offset.Y,
                    Width = updatedTilesRect.Width * this.GridSize,
                    Height = updatedTilesRect.Height * this.GridSize,
                };
                this.Invalidate(updatedRect);
                this.Update();
                break;
            }
        }

        private void AdjustScrollBars()
        {
            if (this.Map != null)
            {
                Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                int hMax = this.Map.Width * this.GridSize - this.HScrollBar.Width;
                if (0 < hMax)
                {
                    this.HScrollBar.Enabled = true;
                    this.HScrollBar.Minimum = 0;
                    this.HScrollBar.Maximum = hMax + this.HScrollBar.Width - 1;
                    this.HScrollBar.SmallChange = Util.BackgroundGridSize;
                    this.HScrollBar.LargeChange = this.HScrollBar.Width;
                    Debug.Assert(this.HScrollBar.LargeChange == this.HScrollBar.Width);
                    this.HScrollBar.Value = Math.Min(Math.Max(0, -offset.X), hMax);
                }
                else
                {
                    this.HScrollBar.Enabled = false;
                    this.HScrollBar.Value = 0;
                }
                int vMax = this.Map.Height * this.GridSize - this.VScrollBar.Height;
                if (0 < vMax)
                {
                    this.VScrollBar.Enabled = true;
                    this.VScrollBar.Minimum = 0;
                    this.VScrollBar.Maximum = vMax + this.VScrollBar.Height - 1;
                    this.VScrollBar.SmallChange = Util.BackgroundGridSize;
                    this.VScrollBar.LargeChange = this.VScrollBar.Height;
                    Debug.Assert(this.VScrollBar.LargeChange == this.VScrollBar.Height);
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
        private int RenderingTileStartX = 0;
        private int RenderingTileStartY = 0;

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            if (this.Map != null)
            {
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
                        this.RenderingTileStartX = x;
                        this.RenderingTileStartY = y;
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
                    this.Invalidate(oldFrameRect);
                    this.Invalidate(this.FrameRect);
                    this.Update();
                }
            }
        }

        private Rectangle PreviousFrameRect = Rectangle.Empty;
        private Point MapOffsetWhenFrameRectSaved = Point.Empty;

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (this.EditorState.LayerMode != LayerMode.Event)
            {
                Rectangle previousFrameRect = this.PreviousFrameRect;
                if (this.Map != null)
                {
                    Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                    previousFrameRect.X += -this.MapOffsetWhenFrameRectSaved.X + offset.X;
                    previousFrameRect.Y += -this.MapOffsetWhenFrameRectSaved.Y + offset.Y;
                    Point mousePosition = new Point
                    {
                        X = e.X - offset.X,
                        Y = e.Y - offset.Y,
                    };
                    this.CursorTileX =
                        Math.Min(Math.Max(mousePosition.X / this.GridSize, 0), this.Map.Width - 1);
                    this.CursorTileY =
                        Math.Min(Math.Max(mousePosition.Y / this.GridSize, 0), this.Map.Height - 1);
                    if (this.IsPickingTiles)
                    {
                        if ((e.Button & MouseButtons.Right) != 0)
                        {
                            if (previousFrameRect != this.FrameRect)
                            {
                                this.Invalidate(previousFrameRect);
                                this.Invalidate(this.FrameRect);
                                this.Update();
                            }
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
                            if (previousFrameRect != this.FrameRect)
                            {
                                this.Invalidate(previousFrameRect);
                            }
                            if (!this.Map.SetTiles(layer, x, y, selectedTiles,
                                x - this.RenderingTileStartX, y - this.RenderingTileStartY))
                            {
                                if (previousFrameRect != this.FrameRect)
                                {
                                    this.Invalidate(this.FrameRect);
                                }
                            }
                            this.Update();
                        }
                        else
                        {
                            if (this.EditorState.LayerMode != LayerMode.Event)
                            {
                                if (previousFrameRect != this.FrameRect)
                                {
                                    this.Invalidate(previousFrameRect);
                                    this.Invalidate(this.FrameRect);
                                    this.Update();
                                }
                            }
                        }
                    }
                }
            }
            this.PreviousFrameRect = this.FrameRect;
            if (this.Map != null)
            {
                this.MapOffsetWhenFrameRectSaved = this.EditorState.GetMapOffset(this.Map.Id);
            }
            else
            {
                this.MapOffsetWhenFrameRectSaved = Point.Empty;
            }
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            if (this.Map != null && this.IsPickingTiles)
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
            if (this.Map != null)
            {
                Rectangle previousFrameRect = this.PreviousFrameRect;
                Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                previousFrameRect.X += -this.MapOffsetWhenFrameRectSaved.X + offset.X;
                previousFrameRect.Y += -this.MapOffsetWhenFrameRectSaved.Y + offset.Y;
                if (previousFrameRect != this.FrameRect)
                {
                    this.Invalidate(previousFrameRect);
                }
                this.Invalidate(this.FrameRect);
                this.Update();
                this.PreviousFrameRect = Rectangle.Empty;
            }
        }

        protected override void OnMouseWheel(MouseEventArgs e)
        {
            base.OnMouseWheel(e);
            if (this.Map != null)
            {
                Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                if ((Control.ModifierKeys & Keys.Shift) != 0)
                {
                    offset.X += (e.Delta / 120) * this.HScrollBar.SmallChange;
                }
                else
                {
                    offset.Y += (e.Delta / 120) * this.VScrollBar.SmallChange;
                }
                this.EditorState.SetMapOffset(this.Map.Id, offset);
            }
        }

        private int GridSize
        {
            get
            {
                // TODO
                return Util.BackgroundGridSize;
            }
        }

        private Size OffscreenSize = Size.Empty;
        private IntPtr HOffscreen = IntPtr.Zero;
        private IntPtr HOffscreenDC = IntPtr.Zero;
        private unsafe IntPtr OffscreenPixels = IntPtr.Zero;

        protected override void OnLayout(LayoutEventArgs e)
        {
            base.OnLayout(e);
            this.AdjustScrollBars();
            if (this.HOffscreen != IntPtr.Zero)
            {
                NativeMethods.DeleteDC(this.HOffscreenDC);
                NativeMethods.DeleteObject(this.HOffscreen);
                this.OffscreenPixels = IntPtr.Zero;
                this.HOffscreenDC = IntPtr.Zero;
                this.HOffscreen = IntPtr.Zero;
                this.OffscreenSize = Size.Empty;
            }
            this.OffscreenSize = new Size
            {
                Width = this.HScrollBar.Width,
                Height = this.VScrollBar.Height,
            };
            NativeMethods.BITMAPINFO bitmapInfo = new NativeMethods.BITMAPINFO
            {
                bmiHeader = new NativeMethods.BITMAPINFOHEADER
                {
                    biSize = (uint)Marshal.SizeOf(typeof(NativeMethods.BITMAPINFOHEADER)),
                    biWidth = this.OffscreenSize.Width,
                    biHeight = -this.OffscreenSize.Height,
                    biPlanes = 1,
                    biBitCount = 32,
                    biCompression = NativeMethods.BI_RGB,
                },
            };
            IntPtr hDC = IntPtr.Zero;
            try
            {
                hDC = NativeMethods.GetDC(this.Handle);
                this.HOffscreen = NativeMethods.CreateDIBSection(hDC, ref bitmapInfo,
                    NativeMethods.DIB_RGB_COLORS, out this.OffscreenPixels, IntPtr.Zero, 0);
                this.HOffscreenDC = NativeMethods.CreateCompatibleDC(hDC);
                NativeMethods.SelectObject(this.HOffscreenDC, this.HOffscreen);
            }
            finally
            {
                if (hDC != IntPtr.Zero)
                {
                    NativeMethods.ReleaseDC(this.Handle, hDC);
                    hDC = IntPtr.Zero;
                }
            }
            this.Invalidate();
            this.Update();
        }

        private void UpdateOffscreen()
        {
            this.UpdateOffscreen(new Rectangle(new Point(0, 0), this.OffscreenSize));
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
            Size offscreenSize = this.OffscreenSize;

            int bgGridSize = Util.BackgroundGridSize;
            int bgStartI = Math.Max(-offset.X / bgGridSize, 0);
            int bgEndI = Math.Min((offscreenWidth - offset.X) / bgGridSize + 1, map.Width);
            int bgStartJ = Math.Max(-offset.Y / bgGridSize, 0);
            int bgEndJ = Math.Min((offscreenHeight - offset.Y) / bgGridSize + 1, map.Height);
            
            int tmpBgStartX = bgStartI * bgGridSize + offset.X;
            int tmpBgEndX = bgEndI * bgGridSize + offset.X;
            int tmpBgStartY = bgStartJ * bgGridSize + offset.Y;
            int tmpBgEndY = bgEndJ * bgGridSize + offset.Y;
            int paddingI1, paddingI2, paddingJ1, paddingJ2;
            bool fillBlankSpace = false;
            if (0 <= rect.Left - tmpBgStartX)
            {
                paddingI1 = (rect.Left - tmpBgStartX) / bgGridSize;
            }
            else
            {
                paddingI1 = 0;
                fillBlankSpace = true;
            }
            if (0 <= tmpBgEndX - rect.Right)
            {
                paddingI2 = (tmpBgEndX - rect.Right) / bgGridSize;
            }
            else
            {
                paddingI2 = 0;
                fillBlankSpace = true;
            }
            if (0 <= rect.Top - tmpBgStartY)
            {
                paddingJ1 = (rect.Top - tmpBgStartY) / bgGridSize;
            }
            else
            {
                paddingJ1 = 0;
                fillBlankSpace = true;
            }
            if (0 <= tmpBgEndY - rect.Bottom)
            {
                paddingJ2 = (tmpBgEndY - rect.Bottom) / bgGridSize;
            }
            else
            {
                paddingJ2 = 0;
                fillBlankSpace = true;
            }
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
            tileEndI -= paddingI2 * bgGridSize / tileGridSize;
            tileStartJ += paddingJ1 * bgGridSize / tileGridSize;
            tileEndJ -= paddingJ2 * bgGridSize / tileGridSize;

            using (Graphics g = Graphics.FromHdc(this.HOffscreenDC))
            {
                if (fillBlankSpace)
                {
                    NativeMethods.RECT win32Rect1 = new NativeMethods.RECT
                    {
                        Left = 0,
                        Right = offscreenWidth,
                        Top = map.Height * this.GridSize,
                        Bottom = offscreenHeight,
                    };
                    NativeMethods.FillRect(this.HOffscreenDC, ref win32Rect1, (IntPtr)(NativeMethods.COLOR_BTNFACE + 1));
                    NativeMethods.RECT win32Rect2 = new NativeMethods.RECT
                    {
                        Left = map.Width * this.GridSize,
                        Right = offscreenWidth,
                        Top = 0,
                        Bottom = win32Rect1.Top,
                    };
                    NativeMethods.FillRect(this.HOffscreenDC, ref win32Rect2, (IntPtr)(NativeMethods.COLOR_BTNFACE + 1));
                }
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
                            NativeMethods.BitBlt(
                                this.HOffscreenDC, x, y, bgGridSize, bgGridSize,
                                Util.HBackgroundBitmapDC, 0, 0,
                                NativeMethods.TernaryRasterOperations.SRCCOPY);
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
                    Pen eventGridPen = new Pen(Color.FromArgb(0x20, 0x00, 0x00, 0x00));
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
                            this.DarkenOffscreen(offscreenSize, new Rectangle
                            {
                                X = bgStartI * bgGridSize + offset.X,
                                Y = bgStartJ * bgGridSize + offset.Y,
                                Width = (bgEndI - bgStartI) * bgGridSize,
                                Height = (bgEndJ - bgStartJ) * bgGridSize,
                            });
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
            rect.X = Math.Max(rect.X, 0);
            rect.Y = Math.Max(rect.Y, 0);
            if (dstSize.Width < rect.Right)
            {
                rect.Width -= rect.Right - dstSize.Width;
            }
            if (dstSize.Height < rect.Bottom)
            {
                rect.Height -= rect.Bottom - dstSize.Height;
            }
            int stride = (dstSize.Width * 4 + 3) / 4 * 4;
            int padding = stride - rect.Width * 4;
            int startI = rect.Left;
            int startJ = rect.Top;
            int endI = rect.Right;
            int endJ = rect.Bottom;
            unsafe
            {
                byte* dst = (byte*)this.OffscreenPixels + startI * 4 + startJ * stride;
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

        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
            case NativeMethods.WM_PAINT:
                NativeMethods.PAINTSTRUCT ps;
                NativeMethods.RECT rect;
                Size offscreenSize = this.OffscreenSize;
                bool renderCorner = false;
                bool test;
                if (test = NativeMethods.GetUpdateRect(m.HWnd, out rect, false))
                {
                    renderCorner = (offscreenSize.Width < rect.Right) &&
                        (offscreenSize.Height < rect.Bottom);
                    if (offscreenSize.Width < rect.Right)
                    {
                        rect.Right = offscreenSize.Width;
                    }
                    if (offscreenSize.Height < rect.Bottom)
                    {
                        rect.Bottom = offscreenSize.Height;
                    }
                }
                else
                {
                    rect.Left = 0;
                    rect.Top = 0;
                    rect.Right = offscreenSize.Width;
                    rect.Bottom = offscreenSize.Height;
                    renderCorner = true;
                }
                this.UpdateOffscreen(rect);
                IntPtr hDstDC = NativeMethods.BeginPaint(m.HWnd, out ps);
                if (this.ViewModel != null && this.EditorState != null && this.Map != null)
                {
                    NativeMethods.BitBlt(
                        hDstDC, rect.Left, rect.Top, rect.Width, rect.Height,
                        this.HOffscreenDC, rect.Left, rect.Top,
                        NativeMethods.TernaryRasterOperations.SRCCOPY);
                }
                else
                {
                    NativeMethods.FillRect(hDstDC, ref rect, (IntPtr)(NativeMethods.COLOR_BTNFACE + 1));
                }
                Rectangle frameRect = this.FrameRect;
                Point mousePosition = this.PointToClient(Control.MousePosition);
                if ((this.EditorState.LayerMode == LayerMode.Event) ||
                    (0 <= mousePosition.X && mousePosition.X < offscreenSize.Width &&
                     0 <= mousePosition.Y && mousePosition.Y < offscreenSize.Height))
                {
                    using (Graphics g = Graphics.FromHdc(hDstDC))
                    {
                        Util.DrawFrame(g, frameRect);
                    }
                }
                if (renderCorner)
                {
                    NativeMethods.RECT cornerRect = new NativeMethods.RECT
                    {
                        Left = offscreenSize.Width,
                        Top = offscreenSize.Height,
                        Right = offscreenSize.Width + this.HScrollBar.Width,
                        Bottom = offscreenSize.Height + this.VScrollBar.Height,
                    };
                    NativeMethods.FillRect(hDstDC, ref cornerRect, (IntPtr)(NativeMethods.COLOR_BTNFACE + 1));
                }
                NativeMethods.EndPaint(m.HWnd, ref ps);
                break;
            }
            base.WndProc(ref m);
        }

        private Rectangle FrameRect
        {
            get
            {
                if (this.Map != null)
                {
                    Point offset = this.EditorState.GetMapOffset(this.Map.Id);
                    int gridSize = this.GridSize;
                    if (!this.IsPickingTiles)
                    {
                        SelectedTiles selectedTiles = this.EditorState.SelectedTiles;
                        int cursorHorizontalCount, cursorVerticalCount;
                        bool isEvent = this.EditorState.LayerMode == LayerMode.Event;
                        if (!isEvent)
                        {
                            cursorHorizontalCount = this.CursorTileX + this.CursorOffsetX;
                            cursorVerticalCount = this.CursorTileY + this.CursorOffsetY;
                        }
                        else
                        {
                            cursorHorizontalCount = this.CursorTileX;
                            cursorVerticalCount = this.CursorTileY;
                        }
                        return new Rectangle
                        {
                            X = cursorHorizontalCount * gridSize + offset.X,
                            Y = cursorVerticalCount * gridSize + offset.Y,
                            Width = gridSize * (!isEvent ? selectedTiles.Width : 1),
                            Height = gridSize * (!isEvent ? selectedTiles.Height : 1),
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
                else
                {
                    return Rectangle.Empty;
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
