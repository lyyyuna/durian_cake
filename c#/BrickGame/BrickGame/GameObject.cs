using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace BrickGame
{
    class GameObject
    {
        public int XPos { get; set; }
        public int YPos { get; set; }

        public BoardDirection Direction { get; set; }

        public int SpeedX { get; set; }
        public int SpeedY { get; set; }

        // 对象载体
        public Rectangle Rect { get; set; }
        public bool m_IsDel = false;

        public virtual void Draw(Graphics g)
        {

        }

        public virtual void Run()
        {

        }
    }
}
