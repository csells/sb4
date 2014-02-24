using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using sb4.ViewModels;
using sb4.Code;

namespace sb4.Controllers {
  public class TinySellsController : SbController {

    public TinySellsController() : base(new sbdb()) { }
    public TinySellsController(ISbDatabase db) : base(db) { }

    // GET: /TinySells/452
    public ActionResult Index(int? code, int? formcode) {
      // NOTE: this allows the code to come from the route or the form
      if (formcode != null) { code = formcode; }

      var link = db.TinyLinks.SingleOrDefault(l => l.Code == code && l.IsActive && !(l.Link == null && l.ISBN == null));
      if (link == null) {
        if (code != null) { ModelState.AddModelError("formcode", string.Format("The code '{0}' is unknown.", code)); }
        return View("Index", new MasterViewModel(db));
      }

      string url = link.Link;
      if (string.IsNullOrEmpty(url)) {
        url = AmazonHelper.GetAmazonLink(link.ISBN);
      }

      Response.Redirect(url, true);
      return null;
    }

  }
}
