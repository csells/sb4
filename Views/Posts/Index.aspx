<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<sb4.ViewModels.PostIndexViewModel>" %>

<asp:Content ID="Content0" ContentPlaceHolderID="HeaderContent" runat="server">
  <link rel="alternate" type="application/atom+xml" title="ATOM 1.0" href="<%= Model.GetFeedLink(Request.Url, "atom10") %>" />
</asp:Content>
<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
  <%= Model.Title %>
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
  <h1>
    <%--    <a href="<%= Model.GetFeedLink(Request.Url, "odata20") %>">
      <img style="width: 28px; height: 28px; float: right; margin: 3px;" src="/content/images/icon-odatafeed-28x28.png"
        alt="<%= Model.GetFeedTitle("OData 2.0") %>" /></a>--%>
    <a href="<%= Model.GetFeedLink(Request.Url, "atom10") %>">
      <img style="width: 28px; height: 28px; float: right; margin: 3px;" src="/content/images/icon-feed-28x28.png"
        alt="<%= Model.GetFeedTitle("ATOM 1.0") %>" /></a>
    <a href="http://twitter.com/csells">
      <img style="width: 28px; height: 28px; float: right; margin: 3px;" src="/content/images/icon-twitter-28x28.png"
        alt="csells on twitter" /></a>
  </h1>
  <p>
    <%= Model.About %>
  </p>
  <% var allPostCategories = Model.PostCategories.ToList(); %>
  <% foreach (var post in Model.Posts) {
  %>
  <br />
  <br />
  <!-- post -->
  <p class="postDate">
    <%= post.CreationDate.ToString("dddd, MMM d, yyyy, h:mm tt") %>
    <% var thisPostCategories = post.GetPostCategories(allPostCategories).ToList();
       if (thisPostCategories.Count != 0) {
    %>
    in
    <% foreach (var category in thisPostCategories) { %>
    <%= Html.ActionLink(category.DisplayName, "Index", "Posts", new { category = category.Name }, null) %>
    <% } %>
    <% } %>
  </p>
  <hr />
  <h2>
    <%= Html.ActionLink(post.Title, "Details", "Posts", new { id = post.Id }, null) %></h2>
  <p>
    <%= post.Content %>
  </p>
  <p>
    <%= Html.ActionLink("0 comments", "Details", "Posts", null, null, "disqus_thread", new RouteValueDictionary(new { id = post.Id }), new Dictionary<string, object>() { { "data-disqus-identifier", post.Id } }) %>
  </p>
  <% } %>
  <br />
  <br />
  <hr />
  <p>
    <% if (Model.OlderPostCount > 0) { %>
    <%= Html.ActionLink(Model.OlderPostCount + " older posts", "Index", "Posts", new { category = Model.CurrentCategory, page = Model.Page + 1 }, null)%>
    <% }
       else { %>
    No older posts
    <% } %>
    &nbsp; &nbsp; &nbsp;
    <% if (Model.NewerPostCount > 0) { %>
    <%= Html.ActionLink(Model.NewerPostCount + " newer posts", "Index", "Posts", new { category = Model.CurrentCategory, page = Model.Page - 1 }, null)%>
    <% }
       else { %>
    No newer posts
    <% } %>
  </p>
  <script type="text/javascript">
    // from: https://help.disqus.com/customer/portal/articles/565624
    var disqus_shortname = "sellsbrothers";

    /* * * DON'T EDIT BELOW THIS LINE * * */
    (function () {
      var s = document.createElement('script'); s.async = true;
      s.type = 'text/javascript';
      s.src = 'http://' + disqus_shortname + '.disqus.com/count.js';
      (document.getElementsByTagName('HEAD')[0] || document.getElementsByTagName('BODY')[0]).appendChild(s);
    }());
  </script>
</asp:Content>
