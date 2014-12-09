using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace WordCount
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private Color m_Color;
        private Font m_Font;
        private void Form1_Load(object sender, EventArgs e)
        {
            m_Color = this.richTextBox1.SelectionColor;
            m_Font = this.richTextBox1.SelectionFont;
            this.richTextBox1.Text = "My father was a self-taught mandolin player. He was one of the best string instrument players in our town. He could not read music, but if he heard a tune a few times, he could play it. When he was younger, he was a member of a small country music band. They would play at local dances and on a few occasions would play for the local radio station. He often told us how he had auditioned and earned a position in a band that featured Patsy Cline as their lead singer. He told the family that after he was hired he never went back. Dad was a very religious man. He stated that there was a lot of drinking and cursing the day of his audition and he did not want to be around that type of environment.";
        }

        private void button1_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
            string strRemove = Regex.Replace(richTextBox1.Text, @"[\p{P}*]", " ");
            string[] strWord = Regex.Split(strRemove, @"\s");
            Dictionary<string, int> dicWord2Num = new Dictionary<string, int>();
            foreach (string strTemp in strWord)
            {
                if (strTemp != "")
                {
                    if (false == dicWord2Num.ContainsKey(strTemp))
                    {
                        dicWord2Num.Add(strTemp, 1);
                    }
                    else
                    {
                        int tmp = dicWord2Num[strTemp];
                        dicWord2Num[strTemp] = tmp + 1;

                    }
                }
            }

            var res = dicWord2Num.OrderByDescending(u => u.Value);
            foreach (var kv in res)
            {
                listBox1.Items.Add(kv.Key + "         " + kv.Value);
            }
        }

        private void lisBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            richTextBox1.Select(0, richTextBox1.Text.Length);
            richTextBox1.SelectionColor = m_Color;
            richTextBox1.SelectionFont = m_Font;
            ListBox ListBoxATmp = sender as ListBox;
            int nIndex = ListBoxATmp.SelectedIndex;
            string strTmp = ListBoxATmp.Items[nIndex] as String;
            string[] strWords = strTmp.Split(' ');

            int Index = richTextBox1.Find(strWords[0], RichTextBoxFinds.WholeWord | RichTextBoxFinds.MatchCase);
            int startPos = Index;
            int nextIndex = 0;
            while (nextIndex != startPos)
            {
                richTextBox1.SelectionStart = Index;
                richTextBox1.SelectionLength = strWords[0].Length;
                richTextBox1.SelectionColor = Color.Blue;
                richTextBox1.SelectionFont = new Font("Times New Roman", (float)17, FontStyle.Bold);
                nextIndex = richTextBox1.Find(strWords[0], Index + strWords[0].Length, RichTextBoxFinds.WholeWord | RichTextBoxFinds.MatchCase);
                if (nextIndex == -1)
                {
                    nextIndex = startPos;
                }
                Index = nextIndex;
            }
        }
    }
}
