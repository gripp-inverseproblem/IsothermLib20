// External hyperlinks open in a separate tab. Internal document links stay in place.
// This applies to the HTML manual when published and when opened from file://.
(function () {
  "use strict";
  function markExternalLinks() {
    document.querySelectorAll('a[href]').forEach(function (anchor) {
      var raw = anchor.getAttribute('href');
      if (!raw) return;
      var target;
      try { target = new URL(raw, document.baseURI); }
      catch (error) { return; }
      if (target.protocol !== 'http:' && target.protocol !== 'https:') return;
      // When the manual is opened via file://, every web link is external.
      // For hosted docs, only a different origin opens in a new tab.
      var isExternal = window.location.protocol === 'file:' ||
                       target.origin !== window.location.origin;
      if (isExternal) {
        anchor.setAttribute('target', '_blank');
        anchor.setAttribute('rel', 'noopener noreferrer');
      }
    });
  }
  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', markExternalLinks);
  } else {
    markExternalLinks();
  }
})();
