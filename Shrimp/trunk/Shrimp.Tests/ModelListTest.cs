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
            public string Name { get; private set; }

            public override void Clear()
            {
                this.Name = "";
            }

            public override JToken ToJson()
            {
                return null;
            }

            public override void LoadJson(JToken json)
            {
                this.Name = json["Name"].Value<string>();
            }
        }

        [Test]
        public void TestLoadJson()
        {
            ModelList<MockModel> modelList = new ModelList<MockModel>();
            modelList.LoadJson(new JArray(
                new JObject(
                    new JProperty("Name", "foo")),
                new JObject(
                    new JProperty("Name", "bar"))));
        }
    }
}
