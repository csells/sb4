using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Http;
using System.Web.Mvc;
using System.Web.Routing;

namespace sb4 {
  public class MvcApplication : System.Web.HttpApplication {
    public static void RegisterRoutes(RouteCollection routes) {
      routes.IgnoreRoute("{resource}.axd/{*pathInfo}");

      // Read category routes from the database
      using (var db = new sbdb()) {
        foreach (var category in db.PostCategories.Select(cat => cat.Name)) {
          routes.MapRoute(
            category + " Route",
            category,
            new { controller = "Posts", action = "Index", category = category }
        );
        }
      }

      // tinysells
      routes.MapRoute(
          "tinysells",
          "tinysells/{code}",
          new { controller = "TinySells", action = "Index", code = "" }
      );

      // writing
      routes.MapRoute(
          "writing",
          "writing/{book}",
          new { controller = "Writing", action = "Index", book = "" }
      );

      // Mama Mbelle's
      routes.MapRoute(
          "mbelle",
          "mbelle",
          new { controller = "Mbelle", action = "Index" }
      );

      // Default route
      routes.MapRoute(
          "Default",                                              // Route name
          "{controller}/{action}/{id}",                           // URL with parameters
          new { controller = "Posts", action = "Index", id = "" } // Parameter defaults
      );
    }

    public static void RegisterApis(HttpConfiguration config) {
      // Web API routes
      config.MapHttpAttributeRoutes();

      config.Routes.MapHttpRoute(
          name: "ActionApi",
          routeTemplate: "api/{controller}/{action}/{id}"
      );
    }

    protected void Application_Start() {
      //ControllerBuilder.Current.SetControllerFactory(new SbControllerFactory());
      GlobalConfiguration.Configure(RegisterApis);
      RegisterRoutes(RouteTable.Routes);
    }

  }
}
