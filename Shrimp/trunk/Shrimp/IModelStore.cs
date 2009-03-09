using System;

namespace Shrimp
{
    internal interface IModelStore
    {
        string FilePath { get; }
        void Save(string path);
        bool Load(string path);
        void Clear();
        bool IsDirty { get; }
        event EventHandler IsDirtyChanged;
    }
}
