using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Shrimp
{
    static class Program
    {
        /*
        Root         ::= Element
        Array        ::= '[' Elements ']'
        Elements     ::= Element ',' | Element ',' Elements
        Hash         ::= '{' KeyAndValues '}'
        KeyAndValues ::= KeyAndValue ',' | KeyAndValues ',' KeyAndValuse
        KeyAndValue  ::= Key '=>' Element
        Element      ::= Digits | String | Array | Hash
        */
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }
    }
}
