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
        List<GameObject> m_lstGameObject = new List<GameObject>();
        private Timer m_timer;
        Bitmap m_bitmap = null;

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
            /*
            Graphics g = this.CreateGraphics();
            
            foreach( var go in m_lstGameObject)
            {
                go.Draw(Graphics.FromImage(m_bitmap));
            }
            g.DrawImage(m_bitmap, 0, 0);
            g.Dispose();
            */
        }

        private void Form1_MouseDown(object sender, MouseEventArgs e)
        {
            m_nTop = e.X;
            m_nLeft = e.Y;
            this.Refresh();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            var m_board = new Board(m_nLeft, m_nTop, 5);
            var m_ball = new Ball(m_nLeft, m_nTop, 3, 3);
            m_bitmap = new Bitmap(this.Width, this.Height);

            m_lstGameObject.Add(m_ball);
            m_lstGameObject.Add(m_board);

            m_timer = new Timer();
            m_timer.Interval = 10;
            m_timer.Tick += new EventHandler(timer_tick);
            m_timer.Start();
        }

        public void timer_tick(object sender, EventArgs e)
        {
            foreach (var go in m_lstGameObject)
            {
                if (go is Ball)
                    go.Run();
            }

            Graphics g = this.CreateGraphics();
            
            // g.Clear(Color.Black);
            var g_tmp = Graphics.FromImage(m_bitmap);
            g_tmp.Clear(Color.Black);
            foreach (var go in m_lstGameObject)
            {
                go.Draw(g_tmp);
            }
            g.DrawImage(m_bitmap, 0, 0);
            g.Dispose();
        }

        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            foreach (var go in m_lstGameObject)
            { 
                if (go is Board)
                { 
                    switch (e.KeyCode)
                    {
                        case Keys.Left:
                            {
                                go.Direction = BoardDirection.Left;
                            }
                            break;
                        case Keys.Right:
                            {
                                go.Direction = BoardDirection.Right;
                            }
                            break;

                        default:
                            {
                                go.Direction = BoardDirection.None;
                            }
                            break;
                    }
                    go.Run();
                }
            }
            // this.Refresh();
        }





        




    }
}
