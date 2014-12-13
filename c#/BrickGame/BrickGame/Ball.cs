﻿using System;
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
            using (var sbrush = new SolidBrush(Color.Red))
            {
                var Rect = new Rectangle(XPos, YPos, 20, 20);
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
                SpeedX = (new Random().Next(3, 5));
            if (XPos >= 250)
                SpeedX = -(new Random().Next(3, 5));
            if (YPos <= 100)
                SpeedY = -(new Random().Next(3, 8));
            if (YPos >= 370)
                SpeedY += (new Random().Next(3, 8));
        }
    }
}
