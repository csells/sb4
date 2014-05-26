<%@ Control Language="C#" Inherits="System.Web.Mvc.ViewUserControl" %>
<div id="WLSearchBoxDiv">
  <form method="get" action="http://www.google.com/search">
    <input name="q" style="color: #808080;"
      value="sellsbrothers.com search"
      onfocus="if(this.value==this.defaultValue)this.value=''; this.style.color='black';" onblur="if(this.value=='')this.value=this.defaultValue; " />
    <input type="hidden" name="sitesearch" value="sellsbrothers.com" />
  </form>
</div>
