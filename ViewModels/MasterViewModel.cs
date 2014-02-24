using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Text.RegularExpressions;

namespace sb4.ViewModels {
  public class MasterViewModel {
    public MasterViewModel(ISbDatabase db) {
      Zones = db.LandingZones.Where(z => z.IsActive);
      CurrentZone = GetCurrentZone(Zones);
      PostCategories = db.PostCategories;
      ContactEmail = db.Site.ContactEmail;
      ContactName = db.Site.ContactName;
      Badges = db.Badges.Where(b => b.IsActive).OrderBy(b => b.Order);
      TextAds = db.TextAds;
      BannerAds = db.BannerAds;
      CopyrightNotice = db.Site.CopyrightNotice;
    }

    LandingZone GetCurrentZone(IQueryable<LandingZone> zones) {
      // Get the zone with the longest match URL (should match "/" at the very least)
      return Zones.Where(z => HttpContext.Current.Request.RawUrl.StartsWith(z.RelativeUrl)).OrderByDescending(z => z.RelativeUrl.Length).FirstOrDefault();
    }

    public LandingZone CurrentZone { get; private set; }
    public IQueryable<LandingZone> Zones { get; private set; }
    public IQueryable<Badge> Badges { get; private set; }
    public IQueryable<Ad> TextAds { get; private set; }
    public IQueryable<Ad> BannerAds { get; private set; }
    public string ContactName { get; private set; }
    public string ContactEmail { get; private set; }
    public string CopyrightNotice { get; private set; }
    public IEnumerable<PostCategory> PostCategories { get; private set; }

    public string PageTitle {
      get {
        string displayName = CurrentZone == null ? null : CurrentZone.DisplayName;
        return string.Format("{0}{1}sellsbrothers.com", displayName, displayName == null ? "Welcome to " : " - ");
      }
    }

  }
}