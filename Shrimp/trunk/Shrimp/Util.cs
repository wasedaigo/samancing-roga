using System;
using System.Collections.Generic;
using System.Drawing;
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
    }
}
