﻿<%@ Control Language="C#" Inherits="System.Web.Mvc.ViewUserControl" %>
<!-- 160x600 ad tag -->
<div data-type="ad" data-publisher="sellsbrothers.com" data-zone="ron" data-format="160x600"></div>
<script type='text/javascript'>
  function _dmBootstrap(file) {
    var _dma = document.createElement('script');
    _dma.type = 'text/javascript';
    _dma.async = true;
    _dma.src = ('https:' == document.location.protocol ? 'https://' : 'http://') + file;
    (document.getElementsByTagName('head')[0] || document.getElementsByTagName('body')[0]).appendChild(_dma);
  }
  function _dmFollowup(file) { if (typeof DMAds === 'undefined') _dmBootstrap('cdn2.DeveloperMedia.com/a.min.js'); }
  (function () { _dmBootstrap('cdn1.DeveloperMedia.com/a.min.js'); setTimeout(_dmFollowup, 2000); })();
</script>
