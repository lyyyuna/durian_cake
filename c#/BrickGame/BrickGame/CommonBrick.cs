using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace BrickGame
{
    class CommonBrick : GameObject
    {
        public Rectangle m_Rect { get; set; }
        public Color m_Color = Color.Blue;
        
        public CommonBrick()
        {

        }

        public CommonBrick(Rectangle r)
        {
            m_Rect = r;
        }

        public override void Draw(Graphics g)
        {
            var p = new Pen(Color.Red, 3);
            using (var sbrush = new SolidBrush(Color.Blue))
            {
                g.DrawRectangle(p, m_Rect);
                g.FillRectangle(sbrush, m_Rect);
            }
        }
    }
}
