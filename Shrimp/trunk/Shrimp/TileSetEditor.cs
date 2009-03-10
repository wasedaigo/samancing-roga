using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Shrimp
{
    internal class TileSetEditor : DatabaseModelEditor
    {
        private System.Windows.Forms.TableLayoutPanel TableLayoutPanel;
    
        public TileSetEditor()
        {
            this.InitializeComponent();
        }

        private void InitializeComponent()
        {
            this.TableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.SuspendLayout();
            // 
            // TableLayoutPanel
            // 
            this.TableLayoutPanel.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Inset;
            this.TableLayoutPanel.ColumnCount = 1;
            this.TableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.TableLayoutPanel.Location = new System.Drawing.Point(176, 8);
            this.TableLayoutPanel.Name = "TableLayoutPanel";
            this.TableLayoutPanel.RowCount = 1;
            this.TableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.TableLayoutPanel.Size = new System.Drawing.Size(616, 584);
            this.TableLayoutPanel.TabIndex = 8;
            // 
            // TileSetEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.Controls.Add(this.TableLayoutPanel);
            this.Name = "TileSetEditor";
            this.Controls.SetChildIndex(this.TableLayoutPanel, 0);
            this.ResumeLayout(false);

        }
    }
}
