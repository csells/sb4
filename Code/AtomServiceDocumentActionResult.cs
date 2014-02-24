using System;
using System.ServiceModel.Syndication;
using System.Web.Mvc;
using System.Xml;

namespace sb4.Code {
  public class ServiceDocumentActionResult : ActionResult {
    ServiceDocument doc;

    public ServiceDocumentActionResult(ServiceDocument doc) {
      this.doc = doc;
    }

    public override void ExecuteResult(ControllerContext context) {
      // NOTE: atompub spec says "application/atomserv+xml",
      // but it causes browsers to download instead of browse
      // and the tools seem to work with "application/xml" just fine
      //context.HttpContext.Response.ContentType = "application/atomserv+xml";
      context.HttpContext.Response.ContentType = "application/xml";
      using (XmlWriter writer = XmlWriter.Create(context.HttpContext.Response.Output)) {
        doc.GetFormatter().WriteTo(writer);
      }
    }
  }
}