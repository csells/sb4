﻿<%@ Control Language="C#" Inherits="System.Web.Mvc.ViewUserControl<sb4.ViewModels.MasterViewModel>" %>
<%@ Import Namespace="sb4" %>

<div>
    <% foreach (Badge badge in Model.Badges) { %>
    <% if (badge.ImagePath == null) { %>
    <!-- text badge -->
    <p style="font-size: x-small">
        <a href="<%= badge.LinkPath %>">
            <%= badge.Title %></a></p>
    <% }
       else { %>
    <!-- graphic badge -->
    <p>
        <a href="<%= badge.LinkPath %>">
            <img src="<%= badge.ImagePath %>" alt="<%= badge.Title %>" /></a></p>
    <% } %>
    <% } %>
</div>
