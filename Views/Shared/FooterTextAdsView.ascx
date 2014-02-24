<%@ Control Language="C#" Inherits="System.Web.Mvc.ViewUserControl<sb4.ViewModels.MasterViewModel>" %>
<%@ Import Namespace="sb4" %>
<!-- text links -->
<p>
  <% foreach (Ad ad in Model.TextAds.Where(ad => ad.InFooter)) { %>
  <a class="footerAdText" href="<%= ad.LinkPath %>">
    <%= ad.Title %></a>
  <% } %>
</p>
