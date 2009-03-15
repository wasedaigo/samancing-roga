﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    partial class TilesPalette : UserControl
    {
        public TilesPalette()
        {
            this.InitializeComponent();
            this.ClientSize = new Size
            {
                Width = Util.GridSize + SystemInformation.VerticalScrollBarWidth,
                Height = this.ClientSize.Height,
            };
            this.VScroll = true;
            this.VerticalScroll.SmallChange = Util.GridSize;
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
            foreach (Bitmap bitmap in this.LargeBitmapCache.Values)
            {
                bitmap.Dispose();
            }
            this.LargeBitmapCache.Clear();
            /*if (this.ViewModel.IsOpened)
            {
                int mapId = this.EditorState.SelectedMapId;
                int tileSetId = this.EditorState.GetSelectedTileSetId(mapId);
                if (this.TileSetCollection.ContainsId(tileSetId))
                {
                    tileSet = this.TileSetCollection.GetItem(tileSetId);
                    this.TileSet = tileSet;
                }
                else
                {
                    this.TileSet = null;
                }
                this.TileSet = this.EditorState.SelectedTileSet;
            }
            else
            {
                this.TileSet = null;
            }*/
            this.TileSet = this.EditorState.SelectedTileSet;
        }

        private void EditorState_Updated(object sender, UpdatedEventArgs e)
        {
            /*switch (e.PropertyName)
            {
            case "SelectedMapId":
                break;
            case "SelectedTileSets":
                break;
            default:
                return;
            }
            int tileSetId = this.EditorState.SelectedTileSetId;
            if (this.TileSetCollection.ContainsId(tileSetId))
            {
                //tileSet = this.TileSetCollection.GetItem(tileSetId);
                this.TileSet = tileSet;
            }
            else
            {
                this.TileSet = null;
            }*/
            this.TileSet = this.EditorState.SelectedTileSet;
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
                    Width = Util.GridSize * 8,
                    Height = this.LargeBitmap.Height,
                };
            }
            else
            {
                this.AutoScrollMinSize = new Size(0, 0);
            }
            
        }

        private Dictionary<TileSet, Bitmap> LargeBitmapCache =
            new Dictionary<TileSet, Bitmap>();

        private Bitmap LargeBitmap
        {
            get
            {
                TileSet tileSet = this.TileSet;
                if (tileSet != null)
                {
                    if (!this.LargeBitmapCache.ContainsKey(tileSet))
                    {
                        using (Bitmap bitmap = new Bitmap(tileSet.ImageFileFullPath))
                        {
                            this.LargeBitmapCache.Add(tileSet, Util.CreateScaledBitmap(bitmap));
                        }
                    }
                    return this.LargeBitmapCache[tileSet];
                }
                else
                {
                    return null;
                }
            }
        }

        /*public Bitmap TilesBitmap
        {
            get
            {
                return this.tilesBitmap;
            }
            set
            {
                if (this.tilesBitmap != value)
                {
                    this.tilesBitmap = value;
                    if (this.DoubleSizedTilesBitmap != null)
                    {
                        this.DoubleSizedTilesBitmap.Dispose();
                        this.DoubleSizedTilesBitmap = null;
                    }
                    if (this.tilesBitmap != null)
                    {

                    }
                    this.UpdateState();
                }
            }
        }
        private Bitmap tilesBitmap;

        private Bitmap DoubleSizedTilesBitmap;*/

        /*private void UpdateState()
        {
            this.Invalidate();
            if (this.DoubleSizedTilesBitmap != null)
            {
                this.AutoScrollMinSize = new Size
                {
                    Width = Util.GridSize * 8,
                    Height = this.DoubleSizedTilesBitmap.Height,
                };
            }
            else
            {
                this.AutoScrollMinSize = new Size(0, 0);
            }
        }*/

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            if (this.TileSet == null)
            {
                return;
            }
            Graphics g = e.Graphics;
            int baseX = (-this.AutoScrollPosition.X + e.ClipRectangle.X) % Util.GridSize;
            int baseY = (-this.AutoScrollPosition.Y + e.ClipRectangle.Y) % Util.GridSize;
            for (int j = 0; j <= (e.ClipRectangle.Height + baseY) / Util.GridSize; j++)
            {
                for (int i = 0; i <= (e.ClipRectangle.Width + baseX) / Util.GridSize; i++)
                {
                    int x = e.ClipRectangle.X - baseX + i * Util.GridSize;
                    int y = e.ClipRectangle.Y - baseY + j * Util.GridSize;
                    g.DrawImage(Util.BackgroundBitmap, x, y);
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
        }
    }
}
