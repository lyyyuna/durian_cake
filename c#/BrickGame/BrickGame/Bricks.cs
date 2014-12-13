using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace BrickGame
{
    class Bricks : GameObject
    {
        private int _width = 270;
        private int _height = 100;

        public List<CommonBrick> m_Rects { get; set; }
        public Bricks()
        {
            m_Rects = new List<CommonBrick>();

            for (int i = 70; i < 70 + _height; i+=20)
            {
                for (int j = 5; j < _width-5; j+=20)
                {
                    var Rect = new Rectangle(j, i, 18, 18);
                    var cbrick = new CommonBrick(Rect);
                    m_Rects.Add(cbrick);
                }
            }
        }

        public override void Draw(Graphics g)
        {
            foreach (var b in m_Rects)
            {
                b.Draw(g);
            }
        }
    }
}
