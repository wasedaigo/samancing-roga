using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shrimp
{
    internal class EditorState : Model
    {
        public EditorState()
        {
            this.Clear();
        }

        public int SelectedMapId
        {
            get { return this.selectedMapId; }
            set
            {
                if (this.selectedMapId != value)
                {
                    this.selectedMapId = value;
                    this.OnUpdated(new UpdatedEventArgs("SelectedMapId"));
                }
            }
        }
        private int selectedMapId;

        public override void Clear()
        {
            this.SelectedMapId = 0;
        }

        public override JObject ToJson()
        {
            return new JObject(
                new JProperty("SelectedMapId", this.SelectedMapId));
        }

        public override void LoadJson(JObject json)
        {
            this.Clear();
            this.SelectedMapId = json["SelectedMapId"].Value<int>();
            this.OnLoaded(EventArgs.Empty);
        }
    }
}
