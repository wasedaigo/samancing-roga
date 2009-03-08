using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    internal partial class MapEditor : UserControl
    {
        public MapEditor()
        {
            this.InitializeComponent();
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
                        this.viewModel.EditorState.Updated -= this.EditorState_Updated;
                    }
                    this.viewModel = value;
                    if (this.viewModel != null)
                    {
                        this.viewModel.EditorState.Updated += this.EditorState_Updated;
                    }
                }
            }
        }
        private ViewModel viewModel;

        private void EditorState_Updated(object sender, UpdatedEventArgs e)
        {
            switch (e.PropertyName)
            {
            case "SelectedMapId":
                this.Invalidate();
                break;
            default:
                throw new ArgumentException("Invalid property name", "e");
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            Graphics g = e.Graphics;
            /*if (this.ViewModel.SelectedMap != null)
            {
            }*/
        }
    }
}
