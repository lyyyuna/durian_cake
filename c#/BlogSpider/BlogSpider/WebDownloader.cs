using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Text.RegularExpressions;
using System.IO;

namespace BlogSpider
{
    public struct CrawlerItem
    {
        public string strUrl;
        public string strRefer;
    }

    public class PageResult
    {
        public string strPageContent;
        public string strVisitUri;
    }

    class WebDownloader
    {
        private readonly Stack<CrawlerItem> m_Stack = new Stack<CrawlerItem>();

        public string GetPageByHttpWebRequest(string url, Encoding encoding, string strRefer)
        {
            string result = null;

            WebResponse response = null;
            StreamReader strreader = null;

            try
            {
                HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
                request.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)";
                request.Accept = "image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*";

                if (!string.IsNullOrEmpty(strRefer))
                {
                    var u = new Uri(strRefer);
                    request.Referer = u.Host;
                }
                else
                {
                    request.Referer = strRefer;
                }
                request.Method = "GET";
                response = request.GetResponse();
                strreader = new StreamReader(response.GetResponseStream(), encoding);
                result = strreader.ReadToEnd();
            }
            catch(Exception ex)
            {
                result = "";
            }
            finally
            {
                if (strreader != null)
                    strreader.Close();
                if (response != null)
                    response.Close();
            }

            return result;
        }

        public void AddUrlQueue(string strUrl)
        {
            var c = new CrawlerItem();
            c.strRefer = strUrl;
            c.strUrl = strUrl;

            m_Stack.Push(c);
        }

        public void Clearqueue()
        {
            m_Stack.Clear();
        }

        public PageResult ProcessQueue(Encoding encoding)
        {
            var pr = new PageResult();

            if (m_Stack.Count == 0)
            {
                return null;
            }
            else
            {
                var c = m_Stack.Pop();

                string strContent = GetPageByHttpWebRequest(c.strUrl, encoding, c.strRefer);

                pr.strPageContent = strContent;
                pr.strVisitUri = c.strUrl;

            }

            return pr;
        }
    }


}
