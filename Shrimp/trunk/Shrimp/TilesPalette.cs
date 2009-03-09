using System;
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
            this.UpdateState();
        }

        public ViewModel ViewModel { get; set; }

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
                        int width = this.tilesBitmap.Width;
                        int height = this.tilesBitmap.Height;
                        int dWidth = width * 2;
                        int dHeight = height * 2;
                        Bitmap dBitmap = new Bitmap(dWidth, dHeight);
                        BitmapData srcBD = this.tilesBitmap.LockBits(new Rectangle
                        {
                            Width = width, Height = height,
                        }, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                        BitmapData dstBD = dBitmap.LockBits(new Rectangle
                        {
                            Width = dWidth, Height = dHeight,
                        }, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
                        int srcPadding = srcBD.Stride - width * 4;
                        int dstPadding = dstBD.Stride - dWidth * 4;
                        unsafe
                        {
                            byte* src = (byte*)srcBD.Scan0;
                            byte* dst = (byte*)dstBD.Scan0;
                            int dstPadding2 = dWidth * 4 + dstPadding * 2;
                            int dWidthX4 = dWidth * 4;
                            for (int j = 0; j < height;
                                j++, src += srcPadding, dst += dstPadding2)
                            {
                                for (int i = 0; i < width; i++, dst += 4)
                                {
                                    for (int k = 0; k < 4; k++, src++, dst++)
                                    {
                                        dst[0] = *src;
                                        dst[4] = *src;
                                        dst[dWidthX4] = *src;
                                        dst[dWidthX4 + 4] = *src;
                                    }
                                }
                            }
                        }
                        dBitmap.UnlockBits(dstBD);
                        this.tilesBitmap.UnlockBits(srcBD);
                        this.DoubleSizedTilesBitmap = dBitmap;
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
                    Width = Util.GridSize * 8,
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
            g.DrawImage(this.DoubleSizedTilesBitmap,
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
