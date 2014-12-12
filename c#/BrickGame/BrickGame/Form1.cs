using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BrickGame
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            this.textBox1.Text = "";
        }

        private void Form1_MouseMove(object sender, MouseEventArgs e)
        {
            this.Text = string.Format("x={0}, y={1}", e.X, e.Y);
        }

        public int m_nTop = 370;
        public int m_nLeft = 90;
        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            var board = new Board(m_nLeft, m_nTop, 90, 15);
            board.Draw(e.Graphics);
        }

        private void Form1_MouseDown(object sender, MouseEventArgs e)
        {
            m_nTop = e.X;
            m_nLeft = e.Y;
            this.Refresh();
        }



        




    }
}
