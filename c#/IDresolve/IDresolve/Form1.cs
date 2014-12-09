using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Globalization;

namespace IDresolve
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            initDic();
        }

        private Dictionary<string, string> dicNum2Area = new Dictionary<string, string>();
        protected void initDic()
        {
            string strPath = Application.StartupPath;
            strPath = strPath.Replace(@"bin\Debug", "");
            strPath = strPath.Replace(@"bin\Release", "");

            
            string strLine;
            FileStream aFile = new FileStream(strPath + @"\AreaCode.txt", FileMode.Open);
            StreamReader aStream = new StreamReader(aFile, Encoding.UTF8);
            strLine = aStream.ReadLine();
            while(strLine != null)
            {
                if (strLine != null)
                {
                    string[] strArr = strLine.Split(' ');
                    dicNum2Area.Add(strArr[0], strArr[1]);
                }
                strLine = aStream.ReadLine();
            }
            aStream.Close();
            aFile.Close();
            


        }

        private void search()
        {
            if (textBox1.Text.Length != 18)
            {
                MessageBox.Show("输入信息有误！");
                return;
            }

            textBox2.Text = dicNum2Area[textBox1.Text.Substring(0,6)];
            dateTimePicker1.Value = DateTime.ParseExact(textBox1.Text.Substring(6, 8), "yyyyMMdd", CultureInfo.InvariantCulture);
            int iBool = int.Parse(textBox1.Text.Substring(16, 1)) % 2;
            if (0 == iBool)
                textBox4.Text = "女";
            else
                textBox4.Text = "男";

        }

        private void button1_Click(object sender, EventArgs e)
        {
            search();
        }

        private void textBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyData == Keys.Enter)
            {
                search();
            }
        }
    }
}
