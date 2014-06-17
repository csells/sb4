using System.Data.Services;
using System.Data.Services.Common;
using System.Linq;
using System.ServiceModel;
using System.ServiceModel.Web;

namespace sb4 {
  [ServiceBehavior(IncludeExceptionDetailInFaults = true)]
  public class odata : DataService<sellsbrothersEntities> {

    public static void InitializeService(DataServiceConfiguration config) {
      config.SetEntitySetAccessRule("Posts", EntitySetRights.AllRead);
      config.SetEntitySetAccessRule("Comments", EntitySetRights.AllRead);
      config.SetEntitySetAccessRule("TinyLinks", EntitySetRights.AllRead);
      config.SetServiceOperationAccessRule("*", ServiceOperationRights.AllRead);
      config.DataServiceBehavior.MaxProtocolVersion = DataServiceProtocolVersion.V2;
      using (var db = new sbdb()) {
        config.SetEntitySetPageSize("Posts", db.Site.PageSize);
      }
    }

    protected override sellsbrothersEntities CreateDataSource() {
      var dataSource = new sellsbrothersEntities();
      dataSource.ContextOptions.ProxyCreationEnabled = false;
      return dataSource;
    }

    [WebGet]
    public bool RejectComment(int id) {
      BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdminOData();

      // clear the IsApproved bit
      var db = CurrentDataSource;
      var comment = db.Comments.SingleOrDefault(c => c.Id == id);
      if (comment == null) { throw new DataServiceException("comment not found: id= " + id.ToString()); }
      comment.IsApproved = false;
      db.SaveChanges();
      return true;
    }

    [WebGet]
    public bool AcceptComment(int id) {
      BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdminOData();

      // set the IsApproved bit
      var db = CurrentDataSource;
      var comment = db.Comments.SingleOrDefault(c => c.Id == id);
      if (comment == null) { throw new DataServiceException("comment not found: id= " + id.ToString()); }
      comment.IsApproved = true;
      db.SaveChanges();
      return true;
    }

  }

}
