using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Shrimp
{
    internal interface ITree
    {
        int Root { get; }
        string GetName(int id);
        int GetParent(int id);
        int[] GetChildren(int id);

        void Add(int parentId);
        void Remove(int id);

        event EventHandler Loaded;
        event EventHandler Cleared;
        event EventHandler<NodeEventArgs> NodeAdded;
        event EventHandler<NodeEventArgs> NodeRemoved;
        event EventHandler<NodeEventArgs> NodeNameChanged;
    }

    internal class NodeEventArgs : EventArgs
    {
        public NodeEventArgs(int nodeId)
        {
            this.NodeId = nodeId;
        }

        public int NodeId { get; private set; }
    }
}
