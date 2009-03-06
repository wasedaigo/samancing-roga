using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Shrimp
{
    internal interface IProject : IModel
    {
        string GameTitle { get; set; }

        event EventHandler GameTitleChanged;
    }
}
