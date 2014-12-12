using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace BrickGame
{
    /// <summary>
    /// 
    /// </summary>
    public enum BoardDirection
    {
        Left, Right, None
    }

    class Board
    {
        public int m_nBoardWidth = 170;
        public int m_nBoardHeight = 15;
        // 坐标
        public int XPos { get; set; }
        public int YPos { get; set; }
        public int SpeedX { get; set; }
        public BoardDirection Direction { get; set; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="nBoardWidth"></param>
        /// <param name="nBoardHeight"></param>
        public Board(int x, int y, int nBoardWidth, int nBoardHeight)
        {
            this.XPos = x;
            this.YPos = y;
            m_nBoardHeight = nBoardHeight;
            m_nBoardWidth = nBoardWidth;

            this.SpeedX = 3;
        }

        public void Draw(Graphics g)
        {
            using (var sbrush = new SolidBrush(Color.LightBlue) )
            {
                var p = new Pen(Color.SaddleBrown);
                var rect = new Rectangle(XPos, YPos, m_nBoardWidth, m_nBoardHeight);
                g.DrawRectangle(p, rect);
                g.FillRectangle(sbrush, rect);
            }

            g.Dispose();
        }

        public void Run()
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
                        if (XPos + m_nBoardWidth < 270)
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
