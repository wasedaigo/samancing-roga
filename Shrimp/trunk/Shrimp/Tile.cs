using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Shrimp
{
    internal struct Tile
    {
        public int TileSetId { get; set; }
        public int TileId { get; set; }

        public override int GetHashCode()
        {
            return this.TileSetId ^ this.TileId;
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }
            else if (!(obj is Tile))
            {
                return false;
            }
            else
            {
                return this.Equals((Tile)obj);
            }
        }

        public bool Equals(Tile tile)
        {
            return this.TileSetId == tile.TileSetId && this.TileId == tile.TileId;
        }

        public static bool operator ==(Tile tile1, Tile tile2)
        {
            return tile1.Equals(tile2);
        }

        public static bool operator !=(Tile tile1, Tile tile2)
        {
            return !(tile1.Equals(tile2));
        }
    }
}
