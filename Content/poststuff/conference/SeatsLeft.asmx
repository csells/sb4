<%@ WebService Language="c#" Class="WebServicesDevCon.WebServicesDevConWebService" %>
using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Web;
using System.Web.Services;

namespace WebServicesDevCon
{
    [WebService(Namespace="http://sellsbrothers.com/WebServicesDevCon/")]
    public class WebServicesDevConWebService : System.Web.Services.WebService
	{
		[WebMethod]
		public int SeatsLeft()
		{
			return 4;
		}
	}
}
