using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Shrimp
{
    internal interface ITree : IModel
    {
        int Root { get; }
        string GetName(int id);
        int GetParent(int id);
        int[] GetChildren(int id);

        void Add(int parentId);
        void Remove(int id);
    }
}
