using System.Collections.Specialized;
using System.Web.Mvc;

namespace sb4.Code {
  public class SimpleActionResult : ActionResult {
    public string ContentType { get; set; }
    public string ResponseOut { get; set; }
    public byte[] ResponseBytes { get; set; }
    public int StatusCode { get; set; }
    public string StatusDescription { get; set; }
    public NameValueCollection Headers { get; private set; }

    public SimpleActionResult() {
      StatusCode = 200;
      StatusDescription = "OK";
      Headers = new NameValueCollection();
    }

    public override void ExecuteResult(ControllerContext context) {
      var response = context.HttpContext.Response;
      response.StatusDescription = StatusDescription;
      response.StatusCode = StatusCode;
      if (ContentType != null) { response.ContentType = ContentType; }
      foreach (var headerName in Headers.AllKeys) {
        response.Headers[headerName] = Headers[headerName];
      }

      if (ResponseOut != null) { response.Write(ResponseOut); }
      if (ResponseBytes != null) { response.BinaryWrite(ResponseBytes); }
      response.Flush();
    }
  }
}