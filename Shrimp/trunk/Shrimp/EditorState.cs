using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal enum MapEditorMode
    {
        Hand,
        Pen,
    }

    internal class EditorState : Model
    {
        public EditorState(ViewModel viewModel)
        {
            this.ViewModel = viewModel;
            this.Clear();
        }

        public ViewModel ViewModel { get; private set; }

        public Map SelectedMap
        {
            get
            {
                Map map;
                if (this.ViewModel.MapCollection.TryGetMap(this.selectedMapId, out map))
                {
                    return map;
                }
                else
                {
                    return null;
                }
            }
        }

        public int SelectedMapId
        {
            get { return this.selectedMapId; }
            set
            {
                if (this.selectedMapId != value)
                {
                    this.selectedMapId = value;
                    this.OnUpdated(new UpdatedEventArgs("SelectedMapId"));
                }
            }
        }
        private int selectedMapId;

        public Point GetMapOffset(int id)
        {
            if (!this.MapOffsets.ContainsKey(id))
            {
                this.MapOffsets.Add(id, new Point(0, 0));
            }
            return this.MapOffsets[id];
        }
        public void SetMapOffset(int id, Point point)
        {
            this.MapOffsets[id] = point;
            this.OnUpdated(new UpdatedEventArgs("MapOffsets", id));
        }
        private Dictionary<int, Point> MapOffsets = new Dictionary<int, Point>();

        public MapEditorMode MapEditorMode
        {
            get { return this.mapEditorMode; }
            set
            {
                if (this.mapEditorMode != value)
                {
                    this.mapEditorMode = value;
                    this.OnUpdated(new UpdatedEventArgs("MapEditorMode"));
                }
            }
        }
        private MapEditorMode mapEditorMode = MapEditorMode.Hand;

        public override void Clear()
        {
            this.SelectedMapId = 0;
            this.MapOffsets.Clear();
        }

        public override JToken ToJson()
        {
            return new JObject(
                new JProperty("SelectedMapId", this.SelectedMapId),
                new JProperty("MapOffsets",
                    new JArray(
                        from p in this.MapOffsets
                        select new JArray(p.Key, new JArray(p.Value.X, p.Value.Y)))),
                new JProperty("MapEditorMode", this.MapEditorMode.ToString()));
        }

        public override void LoadJson(JToken json)
        {
            this.Clear();
            JToken token;
            if ((token = json["SelectedMapId"]) != null)
            {
                this.SelectedMapId = token.Value<int>();
            }
            if ((token = json["MapOffsets"]) != null)
            {
                foreach (JArray x in token as JArray)
                {
                    int id = x[0].Value<int>();
                    Point point = new Point()
                    {
                        X = x[1][0].Value<int>(),
                        Y = x[1][1].Value<int>(),
                    };
                    this.MapOffsets.Add(id, point);
                }
            }
            if ((token = json["MapEditorMode"]) != null)
            {
                this.MapEditorMode = (MapEditorMode)Enum.Parse(typeof(MapEditorMode),
                    token.Value<string>());
            }
            this.OnLoaded(EventArgs.Empty);
        }
    }
}
