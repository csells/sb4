<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<sb4.ViewModels.MasterViewModel>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
  Tiny Sells
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
  <% using (Html.BeginForm("Index", "TinySells", FormMethod.Get)) {%>
  <label for="formcode">Resolve Code:</label>
  <%= Html.TextBox("formcode", "", new { style = "width: 128px" })%>
  <span class="error"><%= Html.ValidationMessage("formcode")%></span>
  <% } %>
</asp:Content>
