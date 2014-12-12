using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace BrickGame
{
    class Ball
    {
        public int XPos { get; set; }
        public int YPos { get; set; }
        public int SpeedX { get; set; }
        public int SpeedY { get; set; }

        // public Rectangle rect;

        public Ball(int x, int y, int speedX, int speedY)
        {
            this.XPos = x;
            this.YPos = y;
            this.SpeedX = speedX;
            this.SpeedY = speedY;
        }

        public void Draw(Graphics g)
        {
            using (var sbrush = new SolidBrush(Color.Red))
            {
                var rect = new Rectangle(XPos, YPos, 20, 20);
                g.DrawEllipse(new Pen(Color.Purple), rect);
                g.FillEllipse(sbrush, rect);

            }
        }

        public void run()
        {
            XPos += SpeedX;
            YPos -= SpeedY;

            if (XPos <= 0)
                SpeedX = (new Random().Next(3, 5));
            if (XPos >= 250)
                SpeedX = -(new Random().Next(3, 5));
            if (YPos <= 70)
                SpeedY = -(new Random().Next(3, 8));
            if (YPos >= 370)
                SpeedY += (new Random().Next(3, 8));
        }
    }
}
