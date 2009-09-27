using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal interface IModel
    {
        void Clear();

        event EventHandler<UpdatedEventArgs> Updated;

        JToken ToJson();
        void LoadJson(JToken json);
    }

    internal class UpdatedEventArgs : EventArgs
    {
        public UpdatedEventArgs(PropertyInfo property)
            : this(property, 0, null, null)
        {
        }

        public UpdatedEventArgs(PropertyInfo property, int itemId, object previousValue, object bounds)
        {
            this.Property = property;
            this.ItemId = itemId;
            this.PreviousValue = previousValue;
            this.Bounds = bounds;
        }

        public PropertyInfo Property { get; private set; }
        public int ItemId { get; private set; }
        public object PreviousValue { get; private set; }
        public object Bounds { get; private set; }
    }

    internal static class IModelExtensions
    {
        public static PropertyInfo GetProperty<T, TResult>(this T obj, Expression<Func<T, TResult>> expr) where T : IModel
        {
            return (PropertyInfo)((MemberExpression)(expr.Body)).Member;
        }
    }
}
