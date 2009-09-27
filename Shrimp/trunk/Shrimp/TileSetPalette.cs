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
    partial class TileSetPalette : UserControl
    {
        private enum AutoScrollingState { Up, Down, }

        public TileSetPalette()
        {
            this.InitializeComponent();
            this.VScroll = true;
            this.VerticalScroll.SmallChange = Util.PaletteGridSize;
            this.AutoScrollingTimer.Interval = 20;
            this.AutoScrollingTimer.Tick += delegate
            {
                Debug.Assert(this.IsSelectingTiles);
                int dy = 0;
                switch ((AutoScrollingState)this.AutoScrollingTimer.Tag)
                {
                case AutoScrollingState.Up:
                    dy = -8;
                    break;
                case AutoScrollingState.Down:
                    dy = +8;
                    break;
                }
                this.AutoScrollPosition = new Point
                {
                    X = -this.AutoScrollPosition.X,
                    Y = -this.AutoScrollPosition.Y + dy,
                };
                this.SetSelectedTileEnd(this.PointToClient(MousePosition));
            };
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
                        this.viewModel.IsOpenedChanged += this.ViewModel_IsOpenedChanged;
                        this.viewModel.EditorState.Updated += this.EditorState_Updated;
                    }
                }
            }
        }
        private ViewModel viewModel;

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

        private TileSetCollection TileSetCollection
        {
            get
            {
                if (this.ViewModel != null)
                {
                    return this.ViewModel.TileSetCollection;
                }
                else
                {
                    return null;
                }
            }
        }

        private void ViewModel_IsOpenedChanged(object sender, EventArgs e)
        {
            if (this.ViewModel.IsOpened)
            {
                this.TileSet = this.EditorState.SelectedTileSet;
            }
            else
            {
                this.TileSet = null;
            }
        }

        private void EditorState_Updated(object sender, UpdatedEventArgs e)
        {
            EditorState editorState = (EditorState)sender;
            if (e.Property == editorState.GetProperty(x => x.SelectedMapId) ||
                e.Property == editorState.GetProperty(x => x.SelectedTileSetIds))
            {
                this.TileSet = this.EditorState.SelectedTileSet;
            }
            else if (e.Property == editorState.GetProperty(x => x.SelectedTiles))
            {
                this.Invalidate();
            }
        }

        private TileSet TileSet
        {
            get { return tileSet; }
            set
            {
                if (this.tileSet != value)
                {
                    if (this.tileSet != null)
                    {
                        this.tileSet.Updated -= this.TileSet_Updated;
                    }
                    this.tileSet = value;
                    if (this.tileSet != null)
                    {
                        this.tileSet.Updated += this.TileSet_Updated;
                    }
                    this.AdjustSize();
                    this.Invalidate();
                }
            }
        }
        private TileSet tileSet;

        private void TileSet_Updated(object sender, EventArgs e)
        {
            this.AdjustSize();
            this.Invalidate();
        }

        private void AdjustSize()
        {
            if (this.TileSet != null)
            {
                this.AutoScrollMinSize = new Size
                {
                    Width = Util.PaletteGridSize * Util.PaletteHorizontalCount,
                    Height = this.LargeBitmap.Height,
                };
            }
            else
            {
                this.AutoScrollMinSize = new Size(0, 0);
            }
        }

        private Bitmap LargeBitmap
        {
            get
            {
                TileSet tileSet = this.TileSet;
                if (tileSet != null)
                {
                    return tileSet.GetBitmap(ScaleMode.Scale1);
                }
                else
                {
                    return null;
                }
            }
        }

        private bool IsSelectingTiles = false;
        private int SelectedTileStartX;
        private int SelectedTileStartY;

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            if (this.TileSet == null)
            {
                return;
            }
            Point point = new Point
            {
                X = e.X - this.AutoScrollPosition.X,
                Y = e.Y - this.AutoScrollPosition.Y,
            };
            this.IsSelectingTiles = true;
            this.SelectedTileStartX =
                Math.Min(Math.Max(point.X / Util.PaletteGridSize, 0), Util.PaletteHorizontalCount - 1);
            this.SelectedTileStartY =
                Math.Min(Math.Max(point.Y / Util.PaletteGridSize, 0), this.TileSet.Height - 1);
            int tileId = this.SelectedTileStartY * Util.PaletteHorizontalCount
                + this.SelectedTileStartX;
            this.EditorState.SelectedTiles = SelectedTiles.Single(new Tile
            {
                TileSetId = (short)this.EditorState.SelectedTileSetId,
                TileId = (short)tileId,
            });
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (this.IsSelectingTiles)
            {
                this.SetSelectedTileEnd(e.Location);
                if (e.Y < 0)
                {
                    this.AutoScrollingTimer.Tag = AutoScrollingState.Up;
                    if (!this.AutoScrollingTimer.Enabled)
                    {
                        this.AutoScrollingTimer.Start();
                    }
                }
                else if (this.ClientSize.Height <= e.Y)
                {
                    this.AutoScrollingTimer.Tag = AutoScrollingState.Down;
                    if (!this.AutoScrollingTimer.Enabled)
                    {
                        this.AutoScrollingTimer.Start();
                    }
                }
                else
                {
                    if (this.AutoScrollingTimer.Enabled)
                    {
                        this.AutoScrollingTimer.Stop();
                    }
                }
            }
        }

        private Timer AutoScrollingTimer = new Timer();

        private void SetSelectedTileEnd(Point mousePoint)
        {
            Debug.Assert(this.IsSelectingTiles);
            Debug.Assert(this.TileSet != null);
            Point point = new Point
            {
                X = mousePoint.X - this.AutoScrollPosition.X,
                Y = mousePoint.Y - this.AutoScrollPosition.Y,
            };
            int selectedTileEndX =
                Math.Min(Math.Max(point.X / Util.PaletteGridSize, 0), Util.PaletteHorizontalCount - 1);
            int selectedTileEndY =
                Math.Min(Math.Max(point.Y / Util.PaletteGridSize, 0), this.TileSet.Height - 1);
            int x = Math.Min(this.SelectedTileStartX, selectedTileEndX);
            int y = Math.Min(this.SelectedTileStartY, selectedTileEndY);
            int tileId = y * Util.PaletteHorizontalCount + x;
            int width = Math.Abs(this.SelectedTileStartX - selectedTileEndX) + 1;
            int height = Math.Abs(this.SelectedTileStartY - selectedTileEndY) + 1;
            this.EditorState.SelectedTiles =
                SelectedTiles.Rectangle(new Tile
                {
                    TileSetId = (short)this.EditorState.SelectedTileSetId,
                    TileId = (short)tileId
                }, width, height);
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            this.AutoScrollingTimer.Stop();
            this.IsSelectingTiles = false;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            if (this.TileSet == null)
            {
                return;
            }
            Graphics g = e.Graphics;
            int baseX = (-this.AutoScrollPosition.X + e.ClipRectangle.X) % Util.PaletteGridSize;
            int baseY = (-this.AutoScrollPosition.Y + e.ClipRectangle.Y) % Util.PaletteGridSize;
            Brush lightBlueBrush = new SolidBrush(Color.FromArgb(0x00, 0x00, 0x80));
            Brush darkBlueBrush = new SolidBrush(Color.FromArgb(0x00, 0x00, 0x40));
            int halfGridSize = Util.PaletteGridSize >> 1;
            for (int j = 0; j <= (e.ClipRectangle.Height + baseY) / Util.PaletteGridSize; j++)
            {
                for (int i = 0; i <= (e.ClipRectangle.Width + baseX) / Util.PaletteGridSize; i++)
                {
                    int x = e.ClipRectangle.X - baseX + i * Util.PaletteGridSize;
                    int y = e.ClipRectangle.Y - baseY + j * Util.PaletteGridSize;
                    g.FillRectangle(lightBlueBrush, new Rectangle
                    {
                        X = x,
                        Y = y,
                        Width = halfGridSize,
                        Height = halfGridSize,
                    });
                    g.FillRectangle(darkBlueBrush, new Rectangle
                    {
                        X = x + halfGridSize,
                        Y = y,
                        Width = halfGridSize,
                        Height = halfGridSize,
                    });
                    g.FillRectangle(darkBlueBrush, new Rectangle
                    {
                        X = x,
                        Y = y + halfGridSize,
                        Width = halfGridSize,
                        Height = halfGridSize,
                    });
                    g.FillRectangle(lightBlueBrush, new Rectangle
                    {
                        X = x + halfGridSize,
                        Y = y + halfGridSize,
                        Width = halfGridSize,
                        Height = halfGridSize,
                    });
                }
            }
            g.DrawImage(this.LargeBitmap,
                e.ClipRectangle.X, e.ClipRectangle.Y,
                new Rectangle
                {
                    X = -this.AutoScrollPosition.X + e.ClipRectangle.X,
                    Y = -this.AutoScrollPosition.Y + e.ClipRectangle.Y,
                    Width = e.ClipRectangle.Width,
                    Height = e.ClipRectangle.Height,
                },
                GraphicsUnit.Pixel);

            SelectedTiles selectedTiles = this.EditorState.SelectedTiles;
            switch (selectedTiles.SelectedTilesType)
            {
            case SelectedTilesType.Single:
            case SelectedTilesType.Rectangle:
                Tile tile = selectedTiles.Tile;
                if (tile.TileSetId == this.EditorState.SelectedTileSetId)
                {
                    int tileId = tile.TileId;
                    Util.DrawFrame(g, new Rectangle
                    {
                        X = tileId % Util.PaletteHorizontalCount * Util.PaletteGridSize
                            + this.AutoScrollPosition.X,
                        Y = tileId / Util.PaletteHorizontalCount * Util.PaletteGridSize
                            + this.AutoScrollPosition.Y,
                        Width = Util.PaletteGridSize * selectedTiles.Width,
                        Height = Util.PaletteGridSize * selectedTiles.Height,
                    });
                }
                break;
            }
        }
    }
}
