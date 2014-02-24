using System;
using System.Configuration;
using System.Data.Services;
using System.Data.Services.Common;
using System.Data.Services.Providers;
using System.ServiceModel;
using ODataBlog;
using sb4.Code;
using System.Linq;
using System.Linq.Expressions;

namespace sb4 {
  [ODataBloggingServiceBehavior(typeof(sellsbrothersEntities), typeof(WindowsLiveWriterBlogAdapter))]
  [EntityAtomMapping("Posts", "CreationDate", "published")]
  [ServiceBehavior(IncludeExceptionDetailInFaults = true)]
  public class WindowsLiveWriter : DataService<sellsbrothersEntities>, IServiceProvider {
    public static void InitializeService(DataServiceConfiguration config) {
      config.SetEntitySetAccessRule("Posts", EntitySetRights.All);
      config.SetEntitySetAccessRule("PostCategories", EntitySetRights.All);
      config.SetEntitySetAccessRule("PostImages", EntitySetRights.All);

      config.UseVerboseErrors = true;
      config.DataServiceBehavior.MaxProtocolVersion = DataServiceProtocolVersion.V2;
    }

    static readonly DateTime minDate = new DateTime(1753, 1, 1); // SQL Server's min date

    [ChangeInterceptor("Posts")]
    public void ChangePost(Post post, UpdateOperations operations) {
      BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdmin();

      switch (operations) {
        case UpdateOperations.Add:
          post.IsActive = true;
          post.UuidString = Guid.NewGuid().ToString();
          if (post.CreationDate < minDate) { post.CreationDate = DateTime.Now; }
          break;

        case UpdateOperations.Change:
          post.IsActive = true;
          break;
      }
    }

    [ChangeInterceptor("PostImages")]
    public void ChangePostImage(PostImage image, UpdateOperations operations) {
      BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdmin();

      if (operations == UpdateOperations.Add) {
        image.SiteId = int.Parse(ConfigurationManager.AppSettings["SiteId"]);
      }
    }

    [ChangeInterceptor("PostCategories")]
    public void ChangePostCategory(PostCategory category, UpdateOperations operations) {
      BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdmin();
    }

    public object GetService(Type serviceType) {
      if (serviceType == typeof(IDataServiceStreamProvider)) {
        return new PictureStreamProvider(CurrentDataSource);
      }
      return null;
    }

  }

}
