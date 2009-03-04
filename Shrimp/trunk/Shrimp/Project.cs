using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;

namespace Shrimp
{
    public class Project
    {
        public void Load(string filename)
        {
            Debug.Assert(File.Exists(filename));
            XmlDocument doc = new XmlDocument();
            doc.Load(filename);
            string dir = Path.GetDirectoryName(filename);
            this.BasePath = Path.GetDirectoryName(dir);
            this.DirectoryName = Path.GetFileName(dir);
            this.GameTitle = doc.SelectSingleNode("/Project/GameTitle").InnerText;
        }

        public void Save()
        {
            string dir = Path.Combine(this.BasePath, this.DirectoryName);
            Debug.Assert(Directory.Exists(dir));
            string filename = Path.Combine(dir, "Game.shrp");
            using (XmlWriter writer = new XmlTextWriter(filename, new UTF8Encoding(false)))
            {
                writer.WriteStartDocument();
                writer.WriteStartElement("Project");

                writer.WriteStartElement("GameTitle");
                writer.WriteString(this.GameTitle);
                writer.WriteEndElement();

                writer.WriteEndElement();
                writer.WriteEndDocument();
            }
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

