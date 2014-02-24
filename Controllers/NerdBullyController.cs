using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace sb4.Controllers {
  public class NerdBullyController : Controller {
    // GET: /NerdBully/
    public ActionResult Index() {
      Response.Redirect("/Content/images/nerdbully.png");
      return null;
    }

  }
}
