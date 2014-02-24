using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using sb4.ViewModels;

namespace sb4.Controllers {
  public class WritingController : SbController {

    public WritingController() : base(new sbdb()) { }
    public WritingController(ISbDatabase db) : base(db) { }

    public ActionResult Index(string book) {
      if (string.IsNullOrEmpty(book)) { return View(new WritingIndexViewModel(db)); }

      // Look up post id using book name as entry name
      var bookPost = db.ActivePosts.SingleOrDefault(p => string.Compare(p.PostName, book, true) == 0 && p.Categories.Contains("book"));
      if (bookPost == null) { Response.Redirect("/writing"); }
      else { Server.TransferRequest(string.Format(@"/posts/details/{0}", bookPost.Id), false); }
      //else { Response.Redirect(string.Format(@"/posts/details/{0}", bookPost.Id), false); }

      return null;
    }

  }
}
