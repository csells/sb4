using System;
using System.ServiceModel.Syndication;
using System.Web.Mvc;
using System.Xml;

namespace sb4.Code {
  public class AtomActionResult : ActionResult {
    SyndicationFeed feed;

    public AtomActionResult(SyndicationFeed feed) {
      this.feed = feed;
    }

    public override void ExecuteResult(ControllerContext context) {
      context.HttpContext.Response.ContentType = "application/atom+xml";
      using (XmlWriter writer = XmlWriter.Create(context.HttpContext.Response.Output)) {
        feed.SaveAsAtom10(writer);
      }
    }
  }
}