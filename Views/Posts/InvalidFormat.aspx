<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
  Invalid Format
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
  <h1>
    Invalid Format</h1>
  <p>
    I'm sorry, but the format
    '<%= ViewData["format"] %>'
    you requested is not supported. You couldn't pick one of the many I do support?!</p>
  <ul>
    <li>ATOM 1.0: <a href="/posts?format=atom">atom</a>, <a href="/posts?format=atom1">atom1</a>, <a href="/posts?format=atom10">atom10</a></li>
    <li>RSS 2.0: <a href="/posts?format=rss">rss</a>, <a href="/posts?format=rss2">rss2</a>, <a href="/posts?format=rss20">rss20</a></li>
    <li>XHTML 1.0 Strict: <a href="/posts">(none)</a>, <a href="/posts?format=html">html</a>, <a href="/posts?format=xhtml">xhtml</a></li>
  </ul>
  <p>Also, you can add an option category and page if you like, e.g.:</p>
  <p><a href="/posts?format=html&page=4&category=spout">posts?format=html&page=4&category=spout</a></p>
</asp:Content>
