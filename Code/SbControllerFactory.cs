using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using sb4.Controllers;

namespace sb4 {
  public class SbControllerFactory : DefaultControllerFactory {

    protected override IController GetControllerInstance(System.Web.Routing.RequestContext requestContext, Type controllerType) {
      if (typeof(SbController).IsAssignableFrom(controllerType)) {
        return (IController)Activator.CreateInstance(controllerType, new sbdb());
      }
      return base.GetControllerInstance(requestContext, controllerType);
    }
  }
}