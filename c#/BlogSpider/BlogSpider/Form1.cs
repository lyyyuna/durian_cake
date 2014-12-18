using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BlogSpider
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        WebDownloader m_Wd = new WebDownloader();
        List<string> m_lstUrls = new List<string>();
        string m_StrCnblogs = "";

        TaskDelegate deles;

        void GatherInitCnblogs()
        {
            string strPagePre = "http://www.cnblogs.com/";
            string strPagePost = "/default.html?page={0}&OnlyTitle=1";
            string strPage = strPagePre + this.textBox1.Text + strPagePost;

            for (int i = 500; i > 0; i--)
            {
                string strTmp = string.Format(strPage, i);
                m_Wd.AddUrlQueue(strTmp);
            }
        }
    }
}
