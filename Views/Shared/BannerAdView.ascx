<%@ Control Language="C#" Inherits="System.Web.Mvc.ViewUserControl<sb4.ViewModels.MasterViewModel>" %>
<%@ Import Namespace="sb4" %>
<div>
  <%
    // Pick a random ad to show
    Ad[] ads = Model.BannerAds.ToArray();
    int count = ads.Count();
    if( count != 0 ) {
      int index = (new Random()).Next(0, count);
      Ad ad = ads.Skip(index).First();
    %>
  <p>
    <a href="<%= ad.LinkPath %>">
      <img src="<%= ad.ImagePath %>" alt="<%= !string.IsNullOrEmpty(ad.Title) ? ad.Title : "" %>" /></a></p>
  <% } %>
</div>
