using System;
using System.ServiceModel.Syndication;
using System.Web.Mvc;
using System.Xml;

namespace sb4.Code {
  public class RssActionResult : ActionResult {
    SyndicationFeed feed;

    public RssActionResult(SyndicationFeed feed) {
      this.feed = feed;
    }

    public override void ExecuteResult(ControllerContext context) {
      context.HttpContext.Response.ContentType = "application/rss+xml";
      using (XmlWriter writer = XmlWriter.Create(context.HttpContext.Response.Output)) {
        feed.GetRss20Formatter(false).WriteTo(writer);
      }
    }
  }
}