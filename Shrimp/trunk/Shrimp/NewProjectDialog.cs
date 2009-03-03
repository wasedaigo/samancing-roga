using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shrimp
{
    public partial class NewProjectDialog : Form
    {
        public NewProjectDialog()
        {
            InitializeComponent();
            string myDocumentPath =
                Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            string defaultPath = Path.Combine(myDocumentPath, "ShrimpProjects");
            this.BasePathTextBox.Text = defaultPath;
            this.ValidateValues();
        }

        public string DirectoryName
        {
            get
            {
                return this.DirectoryNameTextBox.Text;
            }
        }

        public string GameTitle
        {
            get
            {
                return this.GameTitleTextBox.Text;
            }
        }

        public string BasePath
        {
            get
            {
                return this.BasePathTextBox.Text;
            }
        }

        private void FolderPathButton_Click(object sender, EventArgs e)
        {
            this.FolderBrowserDialog.SelectedPath = this.BasePathTextBox.Text;
            if (this.FolderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                this.BasePathTextBox.Text = this.FolderBrowserDialog.SelectedPath;
            }
        }

        private void DirectoryNameTextBox_TextChanged(object sender, EventArgs e)
        {
            this.ValidateValues();
        }

        private void ValidateValues()
        {
            this.ErrorProvider.Clear();
            bool isValidate = true;
            if (Directory.Exists(Path.Combine(this.BasePath, this.DirectoryName)))
            {
                this.ErrorProvider.SetError(this.DirectoryNameTextBox, "Already exists");
                isValidate = false;
            }
            this.OKButton.Enabled = isValidate;
        }
    }
}
