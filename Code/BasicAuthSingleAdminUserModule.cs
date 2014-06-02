// from http://blogs.msdn.com/b/astoriateam/archive/2010/07/21/odata-and-authentication-part-6-custom-basic-authentication.aspx

using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Security.Principal;
using System.Text;
using System.Configuration;
using System.Text.RegularExpressions;
using System.Data.Services;
using System.Security.Cryptography;
using System.Web.Http;
using System.Net.Http;
using System.Net;
using System.Web.Http.Controllers;

namespace sb4 {
  public class BasicAuthSingleAdminUserModule : IHttpModule {

    public void Init(HttpApplication context) {
      context.AuthenticateRequest += new EventHandler(context_AuthenticateRequest);
    }

    void context_AuthenticateRequest(object sender, EventArgs e) {
      HttpApplication application = (HttpApplication)sender;
      bool isAuthenticated = Authenticate(application.Context);

      // NOTE: not all calls need to be authenticated
      //if (!isAuthenticated) {
      //  application.Context.Response.Status = "401 Unauthorized";
      //  application.Context.Response.StatusCode = 401;
      //  application.Context.Response.AddHeader("WWW-Authenticate", "Basic realm=\"sellsbrothers.com\"");
      //  application.CompleteRequest();
      //}
    }

    public void Dispose() { }

    public static bool Authenticate(HttpContext context) {
      if (!HttpContext.Current.Request.IsSecureConnection) { return false; }
      if (!HttpContext.Current.Request.Headers.AllKeys.Contains("Authorization")) { return false; }

      string authHeader = HttpContext.Current.Request.Headers["Authorization"];

      IPrincipal principal;
      if (TryGetPrincipal(authHeader, out principal)) {
        HttpContext.Current.User = principal;
        return true;
      }

      return false;
    }

    static bool TryGetPrincipal(string authHeader, out IPrincipal principal) {
      var creds = ParseAuthHeader(authHeader);
      if (creds != null && TryGetPrincipal(creds, out principal)) { return true; }

      principal = null;
      return false;
    }

    static string[] ParseAuthHeader(string authHeader) {
      // Check this is a Basic Auth header 
      if (authHeader == null || authHeader.Length == 0 || !authHeader.StartsWith("Basic")) { return null; }

      // Pull out the Credentials with are seperated by ':' and Base64 encoded 
      string base64Credentials = authHeader.Substring(6);
      string[] credentials = Encoding.ASCII.GetString(Convert.FromBase64String(base64Credentials)).Split(new char[] { ':' });

      if (credentials.Length != 2 || string.IsNullOrEmpty(credentials[0]) || string.IsNullOrEmpty(credentials[0])) { return null; }

      // Okay this is the credentials 
      return credentials;
    }

    static bool TryGetPrincipal(string[] creds, out IPrincipal principal) {
      string adminUser = ConfigurationManager.AppSettings["AdminUser"];
      string adminPasswordMd5 = ConfigurationManager.AppSettings["AdminPasswordMd5"];

      if (string.Equals(creds[0], adminUser, StringComparison.CurrentCultureIgnoreCase) &&
        string.Equals(creds[1], adminPasswordMd5, StringComparison.CurrentCulture)) {

        principal = new GenericPrincipal(new GenericIdentity(adminUser), new string[] { "Administrator", "User" });
        return true;
      }

      principal = null;
      return false;
    }

    static Regex httpRegex = new Regex("^http://", RegexOptions.IgnoreCase | RegexOptions.Compiled);
    static Regex sbRegex = new Regex("^https://sellsbrothers.com", RegexOptions.IgnoreCase | RegexOptions.Compiled);

    //public static bool ForceSslAndBasicAuthAsAdmin() {
    //  try {
    //    ForceSslAndBasicAuthAsAdminOData();
    //  }
    //  catch (DataServiceException ex) {
    //    HttpResponse response = HttpContext.Current.Response;
    //    response.Status = ex.Message;
    //    response.StatusCode = ex.StatusCode;
    //    response.End();
    //    return false;
    //  }

    //  return true;
    //}

    public static bool ForceSslAndBasicAuthAsAdmin() {
      try {
        ForceSslAndBasicAuthAsAdminCore();
      }
      catch (HttpException ex) {
        HttpResponse response = HttpContext.Current.Response;
        response.Status = ex.Message;
        response.StatusCode = ex.ErrorCode;
        response.End();
        return false;
      }

      return true;
    }

    public static void ForceSslAndBasicAuthAsAdminWebApi(HttpRequestMessage request) {
      try {
        ForceSslAndBasicAuthAsAdminCore();
      }
      catch (HttpException ex) {
        throw new HttpResponseException(request.CreateErrorResponse((HttpStatusCode)ex.WebEventCode, ex));
      }
    }

    static void ForceSslAndBasicAuthAsAdminCore() {
      HttpRequest request = HttpContext.Current.Request;
      HttpResponse response = HttpContext.Current.Response;

      // redirect to SSL to ensure basic auth is not sent over cleartext
      if (!request.IsSecureConnection && !request.IsLocal) {
        // check for sellsbrothers.com vs. www.sellsbrothers.com, since our cert is only www.sellsbrothers.com
        string url = request.Url.ToString();
        url = httpRegex.Replace(url, "https://"); // Replace http with https
        url = sbRegex.Replace(url, "https://www.sellsbrothers.com"); // Replace sellsbrothers.com with www.sellsbrothers.com to make cert work
        response.RedirectPermanent(url, true);
        throw new HttpException(301, "Object moved to " + url.ToString());
      }

      Action Unauthorized = delegate() {
        response.AddHeader("WWW-Authenticate", "Basic realm=\"sellsbrothers.com\"");
        HttpContext.Current.ApplicationInstance.CompleteRequest();
        throw new HttpException(401, "401 Unauthorized");
      };

      // redirect to basic auth if none is provided
      string auth = request.Headers.AllKeys.Contains("Authorization") ? request.Headers["Authorization"].Trim() : null;
      if (string.IsNullOrEmpty(auth) || !auth.StartsWith("basic ", StringComparison.InvariantCultureIgnoreCase)) {

        Unauthorized();
      }

      // authorize
      string adminUser = ConfigurationManager.AppSettings["AdminUser"];
      string adminPasswordMd5 = ConfigurationManager.AppSettings["AdminPasswordMd5"];
      string[] credentials = Encoding.ASCII.GetString(Convert.FromBase64String(auth.Substring(6))).Split(':');
      if (credentials.Length != 2 ||
        !string.Equals(credentials[0], adminUser, StringComparison.CurrentCultureIgnoreCase) ||
        !string.Equals(GetMD5String(credentials[1]), adminPasswordMd5, StringComparison.CurrentCulture)) {

        Unauthorized();
      }
    }

    static string GetMD5String(string s) {
      return (new MD5CryptoServiceProvider()).
        ComputeHash(Encoding.Unicode.GetBytes(s)).
        Aggregate(new StringBuilder(), (working, b) => working.AppendFormat("{0:x2}", b)).
        ToString();
    }

  }

}