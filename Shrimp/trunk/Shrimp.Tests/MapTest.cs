using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Shrimp;

namespace Shrimp.Tests
{
    [TestFixture]
    public class MapTest
    {
        [Test]
        public void TestJson()
        {
            ViewModel viewModel = new ViewModel();
            MapCollection mapCollection = new MapCollection(viewModel);
            Map map1 = new Map(mapCollection);
            map1.Width = 100;
            map1.Height = 200;
            map1.SetTile(0, 1, 2, new Tile { TileSetId = 3, TileId = 4 });
            map1.SetTile(1, 5, 6, new Tile { TileSetId = 7, TileId = 8 });
            JToken token = map1.ToJson();
            Assert.AreEqual(map1.Width, token["Width"].Value<int>());
            Assert.AreEqual(map1.Height, token["Height"].Value<int>());
            Assert.AreEqual(2, token["Tiles"].Count());
            byte[] bytes1 = Convert.FromBase64String(token["Tiles"][0].Value<string>());
            Assert.AreEqual(map1.Width * map1.Height * 4, bytes1.Length);
            Assert.AreEqual(3, bytes1[(1 + 2 * map1.Width) * 4]);
            Assert.AreEqual(0, bytes1[(1 + 2 * map1.Width) * 4 + 1]);
            Assert.AreEqual(4, bytes1[(1 + 2 * map1.Width) * 4 + 2]);
            Assert.AreEqual(0, bytes1[(1 + 2 * map1.Width) * 4 + 3]);
            byte[] bytes2 = Convert.FromBase64String(token["Tiles"][1].Value<string>());
            Assert.AreEqual(map1.Width * map1.Height * 4, bytes2.Length);
            Assert.AreEqual(7, bytes2[(5 + 6 * map1.Width) * 4]);
            Assert.AreEqual(0, bytes2[(5 + 6 * map1.Width) * 4 + 1]);
            Assert.AreEqual(8, bytes2[(5 + 6 * map1.Width) * 4 + 2]);
            Assert.AreEqual(0, bytes2[(5 + 6 * map1.Width) * 4 + 3]);

            Map map2 = new Map(mapCollection);
            Assert.AreEqual(Map.MinWidth, map2.Width);
            Assert.AreEqual(Map.MinHeight, map2.Height);
            map2.LoadJson(token);
            Assert.AreEqual(map1.Width, map2.Width);
            Assert.AreEqual(map1.Height, map2.Height);
            Assert.AreEqual(map1.GetTile(0, 0, 0), map2.GetTile(0, 0, 0));
            Assert.AreEqual(map1.GetTile(0, 1, 2), map2.GetTile(0, 1, 2));
            Assert.AreEqual(map1.GetTile(1, 5, 6), map2.GetTile(1, 5, 6));
        }
    }
}
