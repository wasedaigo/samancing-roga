using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using Shrimp;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp.Tests
{
    [TestFixture]
    public class ModelListTest
    {
        private class MockModel : Model
        {
            public override void Clear()
            {
                this.OnCleared(EventArgs.Empty);
            }

            public override JToken ToJson()
            {
                return null;
            }

            public override void LoadJson(JToken json)
            {
            }
        }

        [Test]
        public void TestCount()
        {
            ModelList<MockModel> modelList = new ModelList<MockModel>();
            Assert.AreEqual(0, modelList.Count);
            modelList.Count = 100;
            Assert.AreEqual(100, modelList.Count);
            modelList.Count = 50;
            Assert.AreEqual(50, modelList.Count);
        }
    }
}
