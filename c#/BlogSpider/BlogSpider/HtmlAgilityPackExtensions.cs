using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Net;
using System.Web;
using HtmlAgilityPack;

namespace BlogSpider
{
    class HtmlAgilityPackExtensions
    {
    }

    static class HtmlToText
    {

    }

    class DocumentWithLinks
    {
        readonly HtmlDocument m_Doc;

        public DocumentWithLinks(HtmlDocument doc)
        {
            m_Doc = doc;

        }
    }
}
