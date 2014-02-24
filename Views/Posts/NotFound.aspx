<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">
	Item Not Found
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">

    <h1>Item Not Found</h1>
    <p>I'm sorry, but the item '<%= ViewData["id"] %>' you requested was not found. Would you like a limerick instead?</p>
    <p />
    <p>There once was a reader from yon<br />
    Whose looked for item was gone.<br />
    Did it perish and wither<br />
    Or scoot off to hither?<br />
    Forsooth we may find it anon.</p>

</asp:Content>
