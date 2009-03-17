using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using Shrimp;

namespace Shrimp.Tests
{
    [TestFixture]
    public class TileTest
    {
        [Test]
        public void Test()
        {
            Tile tile1 = new Tile
            {
                TileSetId = 1,
                TileId = 2,
            };
            Tile tile2 = new Tile
            {
                TileSetId = 1,
                TileId = 2,
            };
            Tile tile3 = new Tile
            {
                TileSetId = 1,
                TileId = 3,
            };
            Assert.IsTrue(Tile.Equals(tile1, tile2));
            Assert.IsFalse(Tile.Equals(tile1, tile3));
            Assert.IsFalse(Tile.Equals(tile1, null));
            Assert.IsTrue(tile1.Equals(tile2));
            Assert.IsFalse(tile1.Equals(tile3));
            Assert.IsFalse(tile1.Equals(null));
            Assert.IsTrue(tile1 == tile2);
            Assert.IsFalse(tile1 != tile2);
            Assert.IsFalse(tile1 == tile3);
            Assert.IsTrue(tile1 != tile3);
            Assert.IsFalse(tile1 == null);
            Assert.IsTrue(tile1 != null);
        }
    }
}
