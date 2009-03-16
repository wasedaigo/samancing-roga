using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    [Flags]
    internal enum MapEditorModes
    {
        Pen = 0x01,
    }

    internal enum SelectedTilesType
    {
        None,
        Single,
        Rectangle,
        Other,
    }

    internal class SelectedTiles
    {
        public static readonly SelectedTiles Empty = None();

        public static SelectedTiles None()
        {
            return new SelectedTiles(SelectedTilesType.None, -1, 0, 0);
        }

        public static SelectedTiles Single(int tileId)
        {
            return new SelectedTiles(SelectedTilesType.Single, tileId, 1, 1);
        }

        public static SelectedTiles Rectangle(int tileId, int width, int height)
        {
            Debug.Assert(0 < width);
            Debug.Assert(tileId % Util.PaletteHorizontalCount + width <= Util.PaletteHorizontalCount);
            Debug.Assert(0 < height);
            var tileIds = Enumerable.Empty<int>();
            for (int j = 0; j < height; j++)
            {
                var line = Enumerable.Range(tileId + j * Util.PaletteHorizontalCount, width);
                tileIds = tileIds.Concat(line);
            }
            return new SelectedTiles(SelectedTilesType.Rectangle, tileIds, width, height);
        }

        public static SelectedTiles Other(IEnumerable<int> tileIds, int width, int height)
        {
            return new SelectedTiles(SelectedTilesType.Other, tileIds, width, height);
        }

        private SelectedTiles(SelectedTilesType type, int tileId, int width, int height)
            : this(type, new[] { tileId }, width, height)
        {
        }

        private SelectedTiles(SelectedTilesType type, IEnumerable<int> tileIds, int width, int height)
        {
            this.SelectedTilesType = type;
            this.TileIds = tileIds;
            this.Width = width;
            this.Height = height;
        }

        public SelectedTilesType SelectedTilesType { get; private set; }
        public int TileId { get { return this.TileIds.First(); } }
        public IEnumerable<int> TileIds { get; private set; }
        public int Width { get; private set; }
        public int Height { get; private set; }
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
            if (this.MapOffsets[id] != point)
            {
                this.MapOffsets[id] = point;
                this.OnUpdated(new UpdatedEventArgs("MapOffsets", id));
            }
        }
        private Dictionary<int, Point> MapOffsets = new Dictionary<int, Point>();

        public int GetSelectedTileSetId(int mapId)
        {
            if (this.ViewModel.MapCollection.Roots.Contains(mapId))
            {
                return -1;
            }
            if (!this.SelectedTileSetIds.ContainsKey(mapId))
            {
                if (0 < this.ViewModel.TileSetCollection.ItemCount)
                {
                    int minId = this.ViewModel.TileSetCollection.ItemIds.Min();
                    this.SelectedTileSetIds.Add(mapId, minId);
                }
                else
                {
                    this.SelectedTileSetIds.Add(mapId, 0);
                }
            }
            return this.SelectedTileSetIds[mapId];
        }
        public void SetSelectedTileSetId(int mapId, int tileSetId)
        {
            if (this.ViewModel.MapCollection.Roots.Contains(mapId))
            {
                throw new ArgumentException("Invalid map ID", "mapId");
            }
            if (this.SelectedTileSetIds[mapId] != tileSetId)
            {
                this.SelectedTileSetIds[mapId] = tileSetId;
                this.OnUpdated(new UpdatedEventArgs("SelectedTileSetIds", mapId));
            }
        }
        private Dictionary<int, int> SelectedTileSetIds = new Dictionary<int, int>();

        public MapEditorModes MapEditorMode
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
        private MapEditorModes mapEditorMode = MapEditorModes.Pen;

        public TileSet SelectedTileSet
        {
            get
            {
                int tileSetId = this.SelectedTileSetId;
                if (this.ViewModel.TileSetCollection.ContainsId(tileSetId))
                {
                    return this.ViewModel.TileSetCollection.GetItem(tileSetId);
                }
                else
                {
                    return null;
                }
            }
        }

        public int SelectedTileSetId
        {
            get
            {
                int mapId = this.SelectedMapId;
                return this.GetSelectedTileSetId(mapId);
            }
        }

        public SelectedTiles SelectedTiles
        {
            get { return this.selectedTiles; }
            set
            {
                if (this.selectedTiles != value)
                {
                    this.selectedTiles = value;
                    this.OnUpdated(new UpdatedEventArgs("SelectedTiles"));
                }
            }
        }
        private SelectedTiles selectedTiles = SelectedTiles.Empty;

        public override void Clear()
        {
            this.SelectedMapId = 0;
            this.MapOffsets.Clear();
            this.SelectedTileSetIds.Clear();
            this.MapEditorMode = MapEditorModes.Pen;
        }

        public override JToken ToJson()
        {
            return new JObject(
                new JProperty("SelectedMapId", this.SelectedMapId),
                new JProperty("MapOffsets",
                    new JArray(
                        from p in this.MapOffsets
                        select new JObject(
                            new JProperty("MapId", p.Key),
                            new JProperty("Offset", new JArray(p.Value.X, p.Value.Y))))),
                new JProperty("SelectedTileSetIds",
                    new JArray(
                        from p in this.SelectedTileSetIds
                        select new JObject(
                            new JProperty("MapId", p.Key),
                            new JProperty("TileSetId", p.Value)))),
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
                foreach (JObject j in token as JArray)
                {
                    int id = j.Value<int>("MapId");
                    int[] jOffset = (j["Offset"] as JArray).Values<int>().ToArray();
                    Point point = new Point()
                    {
                        X = jOffset[0],
                        Y = jOffset[1],
                    };
                    this.MapOffsets.Add(id, point);
                }
            }
            if ((token = json["SelectedTileSetIds"]) != null)
            {
                foreach (JObject j in token as JArray)
                {
                    int mapId = j.Value<int>("MapId");
                    int tileSetId = j.Value<int>("TileSetId");
                    this.SelectedTileSetIds[mapId] = tileSetId;
                }
            }
            if ((token = json["MapEditorMode"]) != null)
            {
                Type type = typeof(MapEditorModes);
                string value = token.Value<string>();
                if (Enum.IsDefined(type, value))
                {
                    this.MapEditorMode = (MapEditorModes)Enum.Parse(type, value);
                }
                else
                {
                    this.MapEditorMode = MapEditorModes.Pen;
                }
            }
        }
    }
}
