using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;

namespace sb4.Controllers {
  public class CommentsController : ApiController {
    protected sellsbrothersEntities CreateDataSource() {
      var dataSource = new sellsbrothersEntities();
      dataSource.ContextOptions.ProxyCreationEnabled = false;
      return dataSource;
    }

    // GET api/comments/approve/{id}
    [HttpGet]
    public bool Approve(int id) {
      BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdminWebApi(this.Request);

      // set the IsApproved bit
      var db = CreateDataSource();
      var comment = db.Comments.SingleOrDefault(c => c.Id == id);
      if (comment == null) { throw new Exception("comment not found: id= " + id.ToString()); }
      comment.IsApproved = true;
      db.SaveChanges();
      return true;
    }

    // GET api/comments/reject/{id}
    [HttpGet]
    public bool Reject(int id) {
      BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdminWebApi(this.Request);

      // clear the IsApproved bit
      var db = CreateDataSource();
      var comment = db.Comments.SingleOrDefault(c => c.Id == id);
      if (comment == null) { throw new Exception("comment not found: id= " + id.ToString()); }
      comment.IsApproved = false;
      db.SaveChanges();
      return true;
    }

  }
}