using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;

namespace Shrimp
{
    internal static class Util
    {
        static Util()
        {
            BackgroundBitmap = new Bitmap(DisplayedGridSize, DisplayedGridSize);
            Graphics g = Graphics.FromImage(BackgroundBitmap);
            Color color1 = Color.FromArgb(0, 0, 0x80);
            Brush brush1 = new SolidBrush(color1);
            Color color2 = Color.FromArgb(0, 0, 0x40);
            Brush brush2 = new SolidBrush(color2);
            g.FillRectangle(brush1, 0, 0, 16, 16);
            g.FillRectangle(brush2, 0, 16, 16, 16);
            g.FillRectangle(brush2, 16, 0, 16, 16);
            g.FillRectangle(brush1, 16, 16, 16, 16);

            IntPtr hBackgroundBitmap = BackgroundBitmap.GetHbitmap();
            IntPtr hBackgroundBitmapDC = g.GetHdc();
            Win32API.SelectObject(hBackgroundBitmapDC, hBackgroundBitmap);

            HBackgroundBitmapDC = hBackgroundBitmapDC;
            gBackgroundBitmap = g;
        }

        public static readonly Bitmap BackgroundBitmap;
        public static readonly IntPtr HBackgroundBitmapDC;
        private static readonly Graphics gBackgroundBitmap; // for GC

        public const int DisplayedGridSize = GridSize * GridScale;
        public const int GridSize = 16;
        public const int GridScale = 2;

        public const int PaletteHorizontalCount = 8;

        public static void CopyDirectory(string src, string dst)
        {
            if (!Directory.Exists(dst))
            {
                Directory.CreateDirectory(dst);
                File.SetAttributes(dst, File.GetAttributes(src));
            }
            foreach (string file in Directory.GetFiles(src))
            {
                string dstFile = Path.Combine(dst, Path.GetFileName(file));
                File.Copy(file, dstFile);
                File.SetAttributes(dstFile, File.GetAttributes(file));
            }
            foreach (string dir in Directory.GetDirectories(src))
            {
                CopyDirectory(dir, Path.Combine(dst, Path.GetFileName(dir)));
            }
        }

        public static int GetNewId(IEnumerable<int> ids)
        {
            int id = 1;
            if (0 < ids.Count())
            {
                int maxId = ids.Max();
                for (int i = id; i <= maxId + 1; i++)
                {
                    if (!ids.Contains(i))
                    {
                        id = i;
                        break;
                    }
                }
            }
            return id;
        }

        public static Bitmap CreateScaledBitmap(Bitmap srcBitmap)
        {
            int srcWidth = srcBitmap.Width;
            int srcHeight = srcBitmap.Height;
            int dstWidth = srcWidth * 2;
            int dstHeight = srcHeight * 2;
            Bitmap dstBitmap = new Bitmap(dstWidth, dstHeight);
            BitmapData srcBD = srcBitmap.LockBits(new Rectangle
            {
                Width = srcWidth,
                Height = srcHeight,
            }, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            BitmapData dstBD = dstBitmap.LockBits(new Rectangle
            {
                Width = dstWidth,
                Height = dstHeight,
            }, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
            int srcPadding = srcBD.Stride - srcWidth * 4;
            int dstPadding = dstBD.Stride - dstWidth * 4;
            unsafe
            {
                byte* src = (byte*)srcBD.Scan0;
                byte* dst = (byte*)dstBD.Scan0;
                int dstPadding2 = dstWidth * 4 + dstPadding * 2;
                int dWidthX4 = dstWidth * 4;
                for (int j = 0; j < srcHeight;
                    j++, src += srcPadding, dst += dstPadding2)
                {
                    for (int i = 0; i < srcWidth; i++, dst += 4)
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
            dstBitmap.UnlockBits(dstBD);
            srcBitmap.UnlockBits(srcBD);
            return dstBitmap;
        }

        public static void DrawBitmap(IntPtr dstPixels, Size dstSize,
            int dstX, int dstY, int width, int height,
            BitmapData srcBD, int srcX, int srcY, byte alpha)
        {
            Debug.Assert(srcBD.PixelFormat == PixelFormat.Format32bppArgb);
            if (width <= 0 || height <= 0)
            {
                return;
            }
            if (dstX < 0)
            {
                width += dstX;
                srcX -= dstX;
                dstX = 0;
            }
            if (srcBD.Width <= srcX)
            {
                return;
            }
            if (dstSize.Width <= dstX + width)
            {
                width -= dstX + width - dstSize.Width;
            }
            if (srcBD.Width <= srcX + width)
            {
                width -= srcX + width - srcBD.Width;
            }
            if (dstY < 0)
            {
                height += dstY;
                srcY -= dstY;
                dstY = 0;
            }
            if (srcBD.Height <= srcY)
            {
                return;
            }
            if (dstSize.Height <= dstY + height)
            {
                height -= dstY + height - dstSize.Height;
            }
            if (srcBD.Height <= srcY + height)
            {
                height -= srcY + height - srcBD.Height;
            }
            if (width <= 0 || height <= 0 ||
                dstX + width <= 0 || dstY + height <= 0 ||
                srcX + width <= 0 || srcY + height <= 0)
            {
                return;
            }
            Debug.Assert(0 <= dstX);
            Debug.Assert(dstX + width <= dstSize.Width);
            Debug.Assert(0 <= dstY);
            Debug.Assert(dstY + height <= dstSize.Height);
            Debug.Assert(0 <= srcX);
            Debug.Assert(srcX + width <= srcBD.Width);
            Debug.Assert(0 <= srcY);
            Debug.Assert(srcY + height <= srcBD.Height);
            unsafe
            {
                int dstStride = (dstSize.Width * 4 + 3) / 4 * 4;
                byte* dst = (byte*)dstPixels + (dstX * 4) + (dstY * dstStride);
                byte* src = (byte*)srcBD.Scan0 + (srcX * 4) + (srcY * srcBD.Stride);
                int paddingDst = dstStride - width * 4;
                int paddingSrc = srcBD.Stride - width * 4;
                Debug.Assert(0 <= paddingDst);
                Debug.Assert(0 <= paddingSrc);
                if (alpha == 255)
                {
                    for (int j = 0; j < height; j++, dst += paddingDst, src += paddingSrc)
                    {
                        for (int i = 0; i < width; i++, dst += 4, src += 4)
                        {
                            byte a = src[3];
                            dst[0] = (byte)(((dst[0] << 8) - dst[0] + (src[0] - dst[0]) * a + 255) >> 8);
                            dst[1] = (byte)(((dst[1] << 8) - dst[1] + (src[1] - dst[1]) * a + 255) >> 8);
                            dst[2] = (byte)(((dst[2] << 8) - dst[2] + (src[2] - dst[2]) * a + 255) >> 8);
                        }
                    }
                }
                else
                {
                    for (int j = 0; j < height; j++, dst += paddingDst, src += paddingSrc)
                    {
                        for (int i = 0; i < width; i++, dst += 4, src += 4)
                        {
                            byte a = (byte)((src[3] * alpha + 255) >> 8);
                            dst[0] = (byte)(((dst[0] << 8) - dst[0] + (src[0] - dst[0]) * a + 255) >> 8);
                            dst[1] = (byte)(((dst[1] << 8) - dst[1] + (src[1] - dst[1]) * a + 255) >> 8);
                            dst[2] = (byte)(((dst[2] << 8) - dst[2] + (src[2] - dst[2]) * a + 255) >> 8);
                        }
                    }
                }
            }
        }

        /*public static void ClearBitmap(BitmapData dstBD)
        {
            Debug.Assert(dstBD.PixelFormat == PixelFormat.Format24bppRgb);
            int size = dstBD.Stride * dstBD.Height;
            unsafe
            {
                byte* dst = (byte*)dstBD.Scan0;
                for (int i = 0; i < size; i++, dst++)
                {
                    *dst = 0;
                }
            }
        }*/

        /*public static void FillBitmap(BitmapData dstBD, Color color)
        {
            Debug.Assert(dstBD.PixelFormat == PixelFormat.Format24bppRgb);
            int width = dstBD.Width;
            int height = dstBD.Height;
            int padding = dstBD.Stride - width * 3;
            byte r = color.R;
            byte g = color.G;
            byte b = color.B;
            unsafe
            {
                byte* dst = (byte*)dstBD.Scan0;
                for (int j = 0; j < height; j++, dst += padding)
                {
                    for (int i = 0; i < width; i++, dst += 3)
                    {
                        dst[0] = b;
                        dst[1] = g;
                        dst[2] = r;
                    }
                }
            }
        }*/

        public static void Darken(IntPtr dstPixels, Size dstSize)
        {
            int width = dstSize.Width;
            int height = dstSize.Height;
            int padding = (dstSize.Width * 4 + 3) / 4 * 4 - width * 4;
            unsafe
            {
                byte* dst = (byte*)dstPixels;
                for (int j = 0; j < height; j++, dst += padding)
                {
                    for (int i = 0; i < width; i++, dst += 4)
                    {
                        dst[0] = (byte)(dst[0] >> 1);
                        dst[1] = (byte)(dst[1] >> 1);
                        dst[2] = (byte)(dst[2] >> 1);
                    }
                }
            }
        }

        private static Pen FramePen1 = new Pen(Color.Black, 1);
        private static Pen FramePen2 = new Pen(Color.White, 2);

        public static void DrawFrame(Graphics g, Rectangle rect)
        {
            g.DrawRectangle(FramePen1, new Rectangle
            {
                X = rect.X,
                Y = rect.Y,
                Width = rect.Width - 1,
                Height = rect.Height - 1,
            });
            g.DrawRectangle(FramePen2, new Rectangle
            {
                X = rect.X + 2,
                Y = rect.Y + 2,
                Width = rect.Width - 4,
                Height = rect.Height - 4,
            });
            g.DrawRectangle(FramePen1, new Rectangle
            {
                X = rect.X + 3,
                Y = rect.Y + 3,
                Width = rect.Width - 7,
                Height = rect.Height - 7,
            });
        }
    }
}
