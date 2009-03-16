using System;
using System.Collections.Generic;
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
            BackgroundBitmap = new Bitmap(32, 32);
            using (Graphics g = Graphics.FromImage(BackgroundBitmap))
            {
                Color color1 = Color.FromArgb(0, 0, 0x80);
                Brush brush1 = new SolidBrush(color1);
                Color color2 = Color.FromArgb(0, 0, 0x40);
                Brush brush2 = new SolidBrush(color2);
                g.FillRectangle(brush1, 0, 0, 16, 16);
                g.FillRectangle(brush2, 0, 16, 16, 16);
                g.FillRectangle(brush2, 16, 0, 16, 16);
                g.FillRectangle(brush1, 16, 16, 16, 16);
            }
        }

        public static readonly Bitmap BackgroundBitmap;

        public const int GridSize = 32;

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
