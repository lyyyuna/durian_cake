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
        bool m_bKeyDown = false;
        Bricks m_bricks;
        Ball m_ball;
        Board m_board;

        public Form1()
        {
            InitializeComponent();
        }

        int score;
        private void panel1_Paint(object sender, PaintEventArgs e)
        {

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
            score = 0;

            m_board = new Board(m_nLeft, m_nTop, 5);
            
            m_ball = new Ball(m_nLeft, m_nTop, 3, 3);
            m_bitmap = new Bitmap(this.Width, this.Height);
            m_bricks = new Bricks();

            m_lstGameObject.Add(m_ball);
            m_lstGameObject.Add(m_board);
            m_lstGameObject.Add(m_bricks);

            m_timer = new Timer();
            m_timer.Interval = 10;
            m_timer.Tick += new EventHandler(timer_tick);
            m_timer.Start();
        }

        /// <summary>
        /// 
        /// </summary>
        private void HitDetect()
        {
            // 砖头与球是否有相交的部分, 及判断球该往哪里反弹
            for (int i = 0; i < m_bricks.m_Rects.Count; i++)
            {
                if (m_ball.Rect.IntersectsWith(m_bricks.m_Rects[i].Rect))
                {
                    var tmp = m_bricks.m_Rects[i].Rect;
                    // m_bricks.m_Rects.Remove(m_bricks.m_Rects[i]);
                    int FocusX = m_ball.Rect.X + m_ball.Rect.Width/2;
                    int FocusY = m_ball.Rect.Bottom + m_ball.Rect.Height/2;
                    if (FocusY > tmp.Bottom && FocusX > tmp.Left && FocusX < tmp.Right)
                    {
                        m_ball.SpeedX = m_ball.SpeedX;
                        m_ball.SpeedY = -m_ball.SpeedY;
                    }
                    else if (FocusY < tmp.Top && FocusX > tmp.Left && FocusX < tmp.Right)
                    {
                        m_ball.SpeedX = m_ball.SpeedX;
                        m_ball.SpeedY = -m_ball.SpeedY;
                    }
                    else if (FocusX < tmp.Left && FocusY > tmp.Top && FocusY < tmp.Bottom)
                    {
                        m_ball.SpeedX = -m_ball.SpeedX;
                        m_ball.SpeedY = m_ball.SpeedY;
                    }
                    else if (FocusX > tmp.Right && FocusY > tmp.Top && FocusY < tmp.Bottom)
                    {
                        m_ball.SpeedX = -m_ball.SpeedX;
                        m_ball.SpeedY = m_ball.SpeedY;
                    }
                    else
                    {
                        m_ball.SpeedX = -m_ball.SpeedX;
                        m_ball.SpeedY = -m_ball.SpeedY;
                    }

                    score += 1;
                    this.textBox1.Text = score.ToString();
                    m_bricks.m_Rects.Remove(m_bricks.m_Rects[i]);
                    break;
                }
            }

            // 检测球是否和挡板碰撞
            if (m_ball.Rect.IntersectsWith(m_board.Rect))
            {
                switch (m_board.Direction)
                {
                    case BoardDirection.Left:
                        {
                            m_ball.SpeedX = -(new Random().Next(1, 2));
                        } break;
                    case BoardDirection.Right:
                        {
                            m_ball.SpeedX = (new Random().Next(1, 2));
                        } break;
                    default:
                        break;
                }
                m_ball.SpeedY = (new Random().Next(1, 2));
            }
        }

        /// <summary>
        /// 定时服务
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void timer_tick(object sender, EventArgs e)
        {
            foreach (var go in m_lstGameObject)
            {
                if (go is Board)
                {
                    if (!m_bKeyDown)
                        continue;
                }
                
                go.Run();
            }

            HitDetect();

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
            m_bKeyDown = true;
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

        private void Form1_KeyUp(object sender, KeyEventArgs e)
        {
            m_bKeyDown = false;
            m_board.Direction = BoardDirection.None;
        }

    }
}
