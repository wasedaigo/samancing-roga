using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;

namespace Shrimp
{
    [JsonObject(MemberSerialization.OptIn)]
    public class Project
    {
        public static Project LoadFile(string filename)
        {
            Debug.Assert(File.Exists(filename));
            string json = File.ReadAllText(filename, new UTF8Encoding(false));
            Project project = JsonConvert.DeserializeObject<Project>(json);
            string dir = Path.GetDirectoryName(filename);
            project.BasePath = Path.GetDirectoryName(dir);
            project.DirectoryName = Path.GetFileName(dir);
            return project;
        }

        public void Save()
        {
            string dir = Path.Combine(this.BasePath, this.DirectoryName);
            Debug.Assert(Directory.Exists(dir));
            string filename = Path.Combine(dir, "Game.shrp");
            string json = JsonConvert.SerializeObject(this, Formatting.Indented);
            File.WriteAllText(filename, json, new UTF8Encoding(false));
            this.OnUpdated(EventArgs.Empty);
        }

        public event EventHandler Updated;

        protected virtual void OnUpdated(EventArgs e)
        {
            if (this.Updated != null)
            {
                this.Updated(this, e);
            }
        }

        public string DirectoryName
        {
            get
            {
                return this.directoryName;
            }
            set
            {
                if (this.directoryName != value)
                {
                    this.directoryName = value;
                    this.OnUpdated(EventArgs.Empty);
                }
            }
        }
        private string directoryName;

        [JsonProperty]
        public string GameTitle
        {
            get
            {
                return this.gameTitle;
            }
            set
            {
                if (this.gameTitle != value)
                {
                    this.gameTitle = value;
                    this.OnUpdated(EventArgs.Empty);
                }
            }
        }
        private string gameTitle;

        public string BasePath
        {
            get
            {
                return this.basePath;
            }
            set
            {
                if (this.basePath != value)
                {
                    this.basePath = value;
                    this.OnUpdated(EventArgs.Empty);
                }
            }
        }
        private string basePath;
    }
}

