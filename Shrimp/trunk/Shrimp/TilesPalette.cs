using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    partial class TilesPalette : UserControl
    {
        private const int GridSize = 32;

        public TilesPalette()
        {
            InitializeComponent();
            this.ClientSize = new Size
            {
                Width = GridSize + SystemInformation.VerticalScrollBarWidth,
                Height = this.ClientSize.Height,
            };
            this.VScroll = true;
            this.VerticalScroll.SmallChange = GridSize;
            this.UpdateState();
        }

        public Bitmap TilesBitmap
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
                        int width = this.tilesBitmap.Width * 2;
                        int height = this.tilesBitmap.Height * 2;
                        this.DoubleSizedTilesBitmap = new Bitmap(width, height);
                        using (Graphics g = Graphics.FromImage(this.DoubleSizedTilesBitmap))
                        {
                            g.InterpolationMode = InterpolationMode.NearestNeighbor;
                            g.ScaleTransform(2, 2);
                            g.DrawImage(this.tilesBitmap, 0, 0);
                        }
                    }
                    this.UpdateState();
                }
            }
        }
        private Bitmap tilesBitmap;

        private Bitmap DoubleSizedTilesBitmap;

        private void UpdateState()
        {
            this.Invalidate();
            if (this.DoubleSizedTilesBitmap != null)
            {
                this.AutoScrollMinSize = new Size
                {
                    Width = GridSize * 8,
                    Height = this.DoubleSizedTilesBitmap.Height,
                };
            }
            else
            {
                this.AutoScrollMinSize = new Size(0, 0);
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            if (this.TilesBitmap == null)
            {
                return;
            }
            Graphics g = e.Graphics;
            g.DrawImage(this.DoubleSizedTilesBitmap,
                e.ClipRectangle.X, e.ClipRectangle.Y,
                new Rectangle
                {
                    X = 0,
                    Y = -this.AutoScrollPosition.Y + e.ClipRectangle.Y,
                    Width = e.ClipRectangle.Width,
                    Height = e.ClipRectangle.Height,
                },
                GraphicsUnit.Pixel);
        }
    }
}
