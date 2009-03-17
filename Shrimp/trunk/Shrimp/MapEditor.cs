using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    internal class GDI32
    {
        public enum TernaryRasterOperations : uint
        {
            SRCCOPY = 0x00CC0020, /* dest = source*/
            SRCPAINT = 0x00EE0086, /* dest = source OR dest*/
            SRCAND = 0x008800C6, /* dest = source AND dest*/
            SRCINVERT = 0x00660046, /* dest = source XOR dest*/
            SRCERASE = 0x00440328, /* dest = source AND (NOT dest )*/
            NOTSRCCOPY = 0x00330008, /* dest = (NOT source)*/
            NOTSRCERASE = 0x001100A6, /* dest = (NOT src) AND (NOT dest) */
            MERGECOPY = 0x00C000CA, /* dest = (source AND pattern)*/
            MERGEPAINT = 0x00BB0226, /* dest = (NOT source) OR dest*/
            PATCOPY = 0x00F00021, /* dest = pattern*/
            PATPAINT = 0x00FB0A09, /* dest = DPSnoo*/
            PATINVERT = 0x005A0049, /* dest = pattern XOR dest*/
            DSTINVERT = 0x00550009, /* dest = (NOT dest)*/
            BLACKNESS = 0x00000042, /* dest = BLACK*/
            WHITENESS = 0x00FF0062, /* dest = WHITE*/
        };

        [DllImport("gdi32.dll")]
        public static extern bool BitBlt(IntPtr hObject, int nXDest, int nYDest, int nWidth, int nHeight, IntPtr hObjSource, int nXSrc, int nYSrc, TernaryRasterOperations dwRop);

        [DllImport("gdi32.dll", ExactSpelling = true, PreserveSig = true, SetLastError = true)]
        public static extern IntPtr SelectObject(IntPtr hdc, IntPtr hgdiobj);

        [DllImport("gdi32.dll")]
        public static extern bool DeleteObject(IntPtr hObject);
    }

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
                    IntPtr hDstDC = g.GetHdc();
                    IntPtr hBackgroundBitmapDC = Util.BackgroundBitmap.GetHbitmap();
                    using (Graphics gBackgroundBitmap = Graphics.FromImage(Util.BackgroundBitmap))
                    {
                        IntPtr hSrcDC = gBackgroundBitmap.GetHdc();
                        IntPtr hBackgroundBitmapDC2 = GDI32.SelectObject(hSrcDC, hBackgroundBitmapDC);
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
                                GDI32.BitBlt(
                                    hDstDC, x, y, Util.DisplayedGridSize, Util.DisplayedGridSize,
                                    hSrcDC, 0, 0, GDI32.TernaryRasterOperations.SRCCOPY);
                            }
                        }
                        GDI32.SelectObject(hSrcDC, hBackgroundBitmapDC2);
                        gBackgroundBitmap.ReleaseHdc(hSrcDC);
                    }
                    g.ReleaseHdc(hDstDC);
                    GDI32.DeleteObject(hBackgroundBitmapDC);
                    int gridSize = Util.GridSize * 2;
                    {
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
                                Tile tile = map.GetTile(0, i, j);
                                int tileId = tile.TileId;
                                if (tileSetCollection.ContainsId(tile.TileSetId))
                                {
                                    if (!bitmaps.ContainsKey(tile.TileSetId))
                                    {
                                        TileSet tileSet = tileSetCollection.GetItem(tile.TileSetId);
                                        bitmaps[tile.TileSetId] = tileSet.GetBitmap(BitmapScale.Scale1);
                                    }
                                    g.DrawImage(bitmaps[tile.TileSetId], x, y, new Rectangle
                                    {
                                        X = (tileId % Util.PaletteHorizontalCount) * gridSize,
                                        Y = (tileId / Util.PaletteHorizontalCount) * gridSize,
                                        Width = gridSize,
                                        Height = gridSize,
                                    }, GraphicsUnit.Pixel);
                                }
                                if (this.EditorState.LayerMode == LayerMode.Event)
                                {
                                    g.DrawRectangle(gridPen, x, y, gridSize - 1, gridSize - 1);
                                }
                            }
                        }
                    }
                    SelectedTiles selectedTiles = this.EditorState.SelectedTiles;
                    Util.DrawFrame(g, new Rectangle
                    {
                        X = this.CursorTileX * gridSize + offset.X,
                        Y = this.CursorTileY * gridSize + offset.Y,
                        Width = gridSize * selectedTiles.Width,
                        Height = gridSize * selectedTiles.Height,
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
