﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Shrimp
{
    /*internal interface ITree : IModel
    {
        int[] Roots { get; }
        string GetName(int id);
        int GetParent(int id);
        int[] GetChildren(int id);

        void Add(int parentId);
        void Remove(int id);
        void Move(int id, int parentId);

        event EventHandler<NodeEventArgs> NodeAdded;
        event EventHandler<NodeEventArgs> NodeRemoved;
        event EventHandler<NodeEventArgs> NodeMoved;
        event EventHandler<NodeEventArgs> NodeNameChanged;
    }*/

    internal class NodeEventArgs : EventArgs
    {
        public NodeEventArgs(int nodeId)
        {
            this.NodeId = nodeId;
        }

        public int NodeId { get; private set; }
    }
}
