using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
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
            this.VScrollBar.Location = new Point
            {
                X = this.MainPanelSize.Width,
                Y = 0,
            };
            this.VScrollBar.Height = this.MainPanelSize.Height;
            this.HScrollBar.Location = new Point
            {
                X = 0,
                Y = this.MainPanelSize.Height,
            };
            this.HScrollBar.Width = this.MainPanelSize.Width;
            /*this.DummyPanel.Location = new Point
            {
                X = this.VScrollBar.Location.X,
                Y = this.HScrollBar.Location.Y,
            };
            this.DummyPanel.Size = new Size
            {
                Width = this.VScrollBar.Width,
                Height = this.HScrollBar.Height
            };*/
        }

        private Size MainPanelSize
        {
            get
            {
                return new Size
                {
                    Width = this.ClientSize.Width - SystemInformation.VerticalScrollBarWidth,
                    Height = this.ClientSize.Height - SystemInformation.HorizontalScrollBarHeight,
                };
            }
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
                        this.viewModel.Opened -= this.ViewModel_Opened;
                        this.viewModel.EditorState.Updated -= this.EditorState_Updated;
                    }
                    this.viewModel = value;
                    if (this.viewModel != null)
                    {
                        this.Map = this.viewModel.EditorState.SelectedMap;
                        this.viewModel.Opened += this.ViewModel_Opened;
                        this.viewModel.EditorState.Updated += this.EditorState_Updated;
                    }
                    else
                    {
                        this.Map = null;
                    }
                }
            }
        }
        private ViewModel viewModel;

        private Map Map
        {
            get { return this.map; }
            set
            {
                if (this.map != value)
                {
                    if (this.map != null)
                    {
                        this.map.Updated -= this.Map_Updated;
                    }
                    this.map = value;
                    if (this.map != null)
                    {
                        this.map.Updated += this.Map_Updated;
                    }
                    this.AdjustScrollBars();
                    this.Invalidate();
                }
            }
        }
        private Map map;

        private int MapId
        {
            get { return this.ViewModel.MapCollection.GetId(this.Map); }
        }

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

        private void ViewModel_Opened(object sender, EventArgs e)
        {
            this.SelectedMapIdChanged();
            this.MapEditorModeChanged();
        }

        private void EditorState_Updated(object sender, UpdatedEventArgs e)
        {
            switch (e.PropertyName)
            {
            case "SelectedMapId":
                this.SelectedMapIdChanged();
                break;
            case "MapEditorMode":
                this.MapEditorModeChanged();
                break;
            case "MapOffsets":
                if ((int)e.Tag == this.ViewModel.MapCollection.GetId(this.Map))
                {
                    this.Invalidate();
                }
                break;
            default:
                throw new ArgumentException("Invalid property name", "e");
            }
        }

        private void SelectedMapIdChanged()
        {
            this.Map = this.EditorState.SelectedMap;
        }

        private void MapEditorModeChanged()
        {
        }

        private void Map_Updated(object sender, UpdatedEventArgs e)
        {
            switch (e.PropertyName)
            {
            case "Width":
            case "Height":
                this.Invalidate();
                break;
            }
        }

        private void AdjustScrollBars()
        {
            this.HScrollBar.SmallChange = 32;
            this.HScrollBar.LargeChange = Math.Max(this.MainPanelSize.Width, 0);
            this.VScrollBar.SmallChange = 32;
            this.VScrollBar.LargeChange = Math.Max(this.MainPanelSize.Height, 0);
            if (this.Map != null)
            {
                int max;
                max = this.Map.Width * 32 - this.MainPanelSize.Width;
                if (0 < max)
                {
                    this.HScrollBar.Enabled = true;
                    this.HScrollBar.Minimum = 0;
                    this.HScrollBar.Maximum = max + this.HScrollBar.LargeChange;
                    if (max < this.HScrollBar.Value)
                    {
                        this.HScrollBar.Value = max;
                    }
                }
                else
                {
                    this.HScrollBar.Value = 0;
                    this.HScrollBar.Enabled = false;
                }
                max = this.Map.Height * 32 - this.MainPanelSize.Height;
                if (0 < max)
                {
                    this.VScrollBar.Enabled = true;
                    this.VScrollBar.Minimum = 0;
                    this.VScrollBar.Maximum = max + this.VScrollBar.LargeChange;
                    if (max < this.VScrollBar.Value)
                    {
                        this.VScrollBar.Value = max;
                    }
                }
                else
                {
                    this.VScrollBar.Value = 0;
                    this.VScrollBar.Enabled = false;
                }
            }
            else
            {
                this.VScrollBar.Enabled = false;
                this.HScrollBar.Enabled = false;
            }
        }

        protected override void OnLayout(LayoutEventArgs e)
        {
            base.OnLayout(e);
            this.AdjustScrollBars();
            this.Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            Graphics g = e.Graphics;
            if (this.ViewModel != null && this.EditorState != null)
            {
                Map map = this.Map;
                if (map != null)
                {
                    Point position = this.EditorState.GetMapOffset(this.MapId);
                    int width = map.Width;
                    int height = map.Height;
                    for (int j = 0; j < height; j++)
                    {
                        int y = j * 32 + position.Y;
                        if (y + 32 <= 0)
                        {
                            continue;
                        }
                        else if (this.MainPanelSize.Height <= y)
                        {
                            break;
                        }
                        for (int i = 0; i < width; i++)
                        {
                            int x = i * 32 + position.X;
                            if (x + 32 <= 0)
                            {
                                continue;
                            }
                            else if (this.MainPanelSize.Width <= x)
                            {
                                break;
                            }
                            g.DrawImage(Util.BackgroundBitmap, x, y);
                        }
                    }
                }
            }
            g.FillRectangle(new SolidBrush(this.BackColor), new Rectangle
            {
                X = this.ClientSize.Width - this.VScrollBar.Width,
                Y = this.ClientSize.Height - this.HScrollBar.Height,
                Width = this.VScrollBar.Width,
                Height = this.HScrollBar.Height,
            });
        }

        private void ScrollBar_ValueChanged(object sender, EventArgs e)
        {
            this.EditorState.SetMapOffset(this.MapId, new Point
            {
                X = -this.HScrollBar.Value,
                Y = -this.VScrollBar.Value,
            });
        }
    }
}
