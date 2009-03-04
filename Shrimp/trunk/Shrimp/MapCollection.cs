using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Serialization;

namespace Shrimp
{
    public class MapCollection
    {
        public class Node
        {
            public Node(Map map)
            {
                this.Map = map;
            }

            public Map Map { get; private set; }
        }

        public Node Root { get; set; }
    }

    public class Map
    {
        public Map(int id)
        {
        }

        public int Id { get; private set; }
    }
}
