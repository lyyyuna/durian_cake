using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace BrickGame
{
    public enum BoardDirection
    {
        Left,
        Right,
        None
    }

    class Board : GameObject
    {
        public int m_BoardWidth = 90;
        public int m_BoardHeight = 15;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="speed"></param>
        public Board(int x, int y, int speed)
        {
            this.XPos = x;
            this.YPos = y;
            this.SpeedX = speed;
        }

        public override void Draw(Graphics g)
        {
            // g.Clear(Color.Black);
            using (var sbrush = new SolidBrush(Color.LightBlue))
            {
                var p = new Pen(Color.SaddleBrown);
                var Rect = new Rectangle(XPos, YPos, m_BoardWidth, m_BoardHeight);
                g.DrawRectangle(p, Rect);
                g.FillRectangle(sbrush, Rect);
            }
        }

        public override void Run()
        {
            switch (Direction)
            {
                case BoardDirection.Left:
                    {
                        if (XPos > 0)
                        {
                            XPos -= SpeedX;
                        }
                    } break;
                case BoardDirection.Right:
                    {
                        if (XPos + m_BoardWidth< 270)
                        {
                            XPos += SpeedX;
                        }
                    } break;
                default:
                    break;
            }
        }
    }
}
