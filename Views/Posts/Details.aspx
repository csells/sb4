﻿<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<sb4.ViewModels.PostDetailsViewModel>" %>

<%@ Import Namespace="Recaptcha.Mvc" %>
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
    <%= Model.Post.Content %></div>
  <!-- comments -->
  <br />
  <br />
  <%
    var comments = Model.Post.Comments.Where(c => c.IsApproved).ToList();
    int commentsCount = comments.Count();
    var commentsTitle = string.Format("{0} {1}", commentsCount, commentsCount == 1 ? "comment" : "comments");
  %>
  <h2>
    <a href="<%= Model.GetCommentFeedLink() %>">
      <img style="width: 28px; height: 28px; float: right; margin: 3px;" src="/content/images/icon-feed-28x28.png"
        alt="Comment Feed" /></a>
    <%= commentsTitle %>
    on this post
  </h2>
  <% foreach (var comment in comments.OrderBy(c => c.CreationDate)) { %>
  <a name="<%= comment.Id %>">
    <h3>
      <%= comment.Author %>:</h3>
  </a>
  <hr />
  <div>
    <%= Model.FormatComment(comment.Content) %></div>
  <p class="commentDate">
    <%= comment.CreationDate.ToString("dddd, MMM d, yyyy, h:mm tt") %></p>
  <br />
  <% } %>
  <!-- post a comment -->
  <br />
  <br />
  <hr />
  <h2>
    comment on this post</h2>
  <% using (Html.BeginForm("CommentSubmitted", "Posts", new { id = Model.Post.Id })) {%>
  <table style="margin: 10px 10px 10px 10px">
    <tr>
      <td />
      <td>
        <%= Html.ValidationSummary(true, "Comment creation was unsuccessful. Please correct the errors and try again.") %>
      </td>
    </tr>
    <tr>
      <td>
        <label for="Author">
          name:</label>
      </td>
      <td>
        <%= Html.TextBox("Author", "", new { size = 40 , maxlength = 64, style = "width: 650px"}) %>
        <span class="error">
          <%= Html.ValidationMessage("Author") %></span>
      </td>
    </tr>
    <%--
      <tr>
        <td>
          <label for="Email">Email:</label>
        </td>
        <td>
          <%= Html.TextBox("Email", "", new { size = 40 , maxlength = 128, style = "width: 650px"}) %>
          <span class="error"><%= Html.ValidationMessage("Email") %></span>
          <%= Html.CheckBox("ShouldEmailNotifications", true) %>
          <label for="ShouldEmailNotifications">
            Email notifications of future comments on this post.</label>
        </td>
      </tr>
    --%>
    <tr>
      <td valign="top">
        <label for="Content">
          comment:</label>
      </td>
      <td>
        <%= Html.TextArea("Content", "", new { rows = 10, cols = 40, style = "width: 650px"}) %>
        <span class="error">
          <%= Html.ValidationMessage("Content") %></span>
      </td>
    </tr>
    <tr>
      <td />
      <td style="font-size: x-small">
        HTML tags will be escaped.
      </td>
    </tr>
    <tr>
      <td />
      <td>
        <%= Html.CreateRecaptcha("recaptcha") %>
        <span class="error">
          <%= Html.ValidationMessage("recaptcha")%></span>
      </td>
    </tr>
    <tr>
      <td />
      <td>
        <input type="submit" value="Post Comment" />
      </td>
    </tr>
  </table>
  <% } %>
</asp:Content>
