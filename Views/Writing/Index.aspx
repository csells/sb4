<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<sb4.ViewModels.WritingIndexViewModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
  Writing & Speaking
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
  <% foreach (var category in Model.OutputCategories) { %>
  <h1>
    <%= category.Name %></h1>
  <ul>
    <% foreach (var item in category.OutputItems.OrderByDescending(i=>i.Date)) { %>
    <li><%= Model.GetItemHtml(item) %><br style="line-height: 20px" /></li>
    <% } %>
  </ul>
  <% } %>
</asp:Content>
