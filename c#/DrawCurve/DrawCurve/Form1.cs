using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DrawCurve
{
    public partial class Form1 : Form
    {
        System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer() { Interval = 200 };
        public Form1()
        {
            InitializeComponent();
            this.DoubleBuffered = true;
            this.timer.Tick += (o, e) => this.Invalidate();
            this.timer.Start();
            //drawSpiral(200);
        }

        List<PointF> points = new List<PointF>();
        protected void drawSpiral(int count)
        {
            int t = 1;
            for (Int32 i = 0; i < count; i++)
            {
                t = i+1;
                float x = t *t/150f * (float)Math.Cos(t / 5f);
                float y = t *t/150f * (float)Math.Sin(t / 5f);

                PointF p = new PointF(this.ClientRectangle.Width /2+x, this.ClientRectangle.Height/2+y);
                points.Add(p);

            }


        }

        protected void drawCyloid(int count)
        {
            float t;
            for (Int32 i = 0; i < count; i++)
            {
                t = i / 10f;
                float x = (float)(100 * Math.Sin(t) + 50 * Math.Sin(t * 20 / 7)) * 1.5f;
                float y = (float)(100 * Math.Cos(t) + 50 * Math.Cos(t * 20 / 7)) * 1.5f;
                PointF p = new PointF(this.ClientRectangle.Width / 2 + x, this.ClientRectangle.Height / 2 + y);
                points.Add(p);
            }
        }

        protected void drawCyloid2(int count)
        {
            float t;
            for (Int32 i = 0; i < count; i++)
            {
                t = i / 10f;
                float x1 = (float)(100 * Math.Sin(t) + 50 * Math.Sin(t * 20 / 7)) * 1.5f;
                float y1 = (float)(100 * Math.Cos(t) + 50 * Math.Cos(t * 20 / 7)) * 1.5f;
                x1 = 0.5f * x1;
                float x = (float)(x1 * Math.Cos(1) - y1 * Math.Sin(1));
                float y = (float)(x1 * Math.Sin(1) + y1 * Math.Cos(1));
                
                PointF p = new PointF(this.ClientRectangle.Width / 2 + x, this.ClientRectangle.Height / 2 + y);
                points.Add(p);
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            drawCyloid2(2000);
                
            e.Graphics.DrawCurve(Pens.Blue, points.ToArray());

            this.timer.Stop();
        }

    }
}
