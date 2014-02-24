using System.Text.RegularExpressions;
using System.Web.Mvc;
using sb4;

namespace sb4.Controllers {
  [HandleError]
  public class SbController : Controller {
    protected readonly ISbDatabase db;

    public SbController(ISbDatabase db) {
      this.db = db;
    }
  }
}
