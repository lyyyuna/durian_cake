using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace BrickGame
{
    class Ball : GameObject
    {
        public Ball(int x, int y, int speedX, int speedY)
        {
            this.XPos = x;
            this.YPos = y;
            this.SpeedX = speedX;
            this.SpeedY = speedY;
        }

        public override void Draw(Graphics g)
        {
            // g.Clear(Color.Black);
            using (var sbrush = new SolidBrush(Color.Red))
            {
                Rect = new Rectangle(XPos, YPos, 10, 10);
                var p = new Pen(Color.Purple);
                g.DrawEllipse(p, Rect);
                g.FillEllipse(sbrush, Rect);
            }
        }

        public override void Run()
        {
            XPos += SpeedX;
            YPos -= SpeedY;

            if (XPos <= 0)
                SpeedX = (new Random().Next(1, 2));
            if (XPos >= 250)
                SpeedX = -(new Random().Next(1, 2));
            if (YPos <= 100)
                SpeedY = -(new Random().Next(1, 2));
            if (YPos >= 380)
                SpeedY += (new Random().Next(1, 2));
        }
    }
}
