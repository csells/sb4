<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<sb4.ViewModels.PostDetailsViewModel>" %>

<%@ Import Namespace="sb4.Controllers" %>
<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
  <%= Model.Post.Title %>
  - sellsbrothers.com
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
  <!-- post -->
  <p class="postDate">
    <% var allPostCategories = Model.AllPostCategories.ToList(); %>
    <%= Model.Post.CreationDate.ToString("dddd, MMM d, yyyy, h:mm tt")%>
    <% var thisPostCategories = Model.Post.GetPostCategories(allPostCategories).ToList();
       if (thisPostCategories.Count != 0) {
    %>
    in
    <% foreach (var category in thisPostCategories) { %>
    <%= Html.ActionLink(category.DisplayName, "Index", "Posts", new { category = category.Name }, null) %>
    <% } %>
    <% } %>
  </p>
  <h1>
    <%= Model.Post.Title %></h1>
  <div>
    <%= Model.Post.Content %>
  </div>
  <!-- comments -->
  <div id="disqus_thread"></div>
  <script type="text/javascript">
    var disqus_shortname = "sellsbrothers";
    var disqus_identifier = <%= Model.Post.Id %>;
    var disqus_title = "<%= Model.Post.Title %>";
    var disqus_url = "<%= Model.CanonicalUrl %>";

    /* * * DON'T EDIT BELOW THIS LINE * * */
    (function () {
      var dsq = document.createElement('script'); dsq.type = 'text/javascript'; dsq.async = true;
      dsq.src = '//' + disqus_shortname + '.disqus.com/embed.js';
      (document.getElementsByTagName('head')[0] || document.getElementsByTagName('body')[0]).appendChild(dsq);
    })();
  </script>
</asp:Content>
