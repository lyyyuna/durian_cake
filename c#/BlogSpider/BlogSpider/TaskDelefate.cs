using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BlogSpider
{
    class DelegatePara
    {
        public int nIsAdsComment;
        public string strLog;
        public string strTitle;
    }

    class TaskDelegate
    {
        public TaskDelegate()
        {

        }

        public ccTaskDelegate Refresh { get; set; }
        public TaskDelegate(ccTaskDelegate refreshDele)
        {
            Refresh += refreshDele;
        }



    }

    delegate void ccTaskDelegate(DelegatePara dp);
}
