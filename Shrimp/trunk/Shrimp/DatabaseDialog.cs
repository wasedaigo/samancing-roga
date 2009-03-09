using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    public partial class DatabaseDialog : Form
    {
        public DatabaseDialog()
        {
            InitializeComponent();
            Font font = this.TileSetLabel.Font;
            this.TileSetLabel.Font = new Font(font.FontFamily, font.Size * 1.5f);
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
