using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data.Objects;
using System.Configuration;

namespace sb4 {
  public class sbdb : IDisposable, ISbDatabase {
    sellsbrothersEntities db = new sellsbrothersEntities();
    Site site;

    // TODO: these should be all IObjectSet and return Site.Foos, e.g. Site.Badges
    // The problem is that the generated code produces ICollection instead of IObjectSet and
    // there's no good way to fix that.
    // TODO: fix up the Posts and Comments properties to do the same as the rest.
    public IQueryable<Badge> Badges { get { return db.Badges.Where(r => r.SiteId == Site.Id); } }
    public IQueryable<Comment> ActiveComments { get { return db.Comments.Where(c => c.IsApproved); } }
    public IQueryable<LandingZone> LandingZones { get { return db.LandingZones.Where(r => r.SiteId == Site.Id); } }
    public IQueryable<Post> ActivePosts { get { return db.Posts.Where(p => p.IsActive); } }
    public Post GetPost(int id) { return db.Posts.Single(p=>p.Id == id); }
    IQueryable<Ad> Ads { get { return db.Ads.Where(a => a.SiteId == Site.Id && !a.IsCanceled && a.ExpirationDate > DateTime.Now); } }
    public IQueryable<Ad> TextAds { get { return Ads.Where(ad => string.IsNullOrEmpty(ad.ImagePath)); } }
    public IQueryable<Ad> BannerAds { get { return Ads.Where(ad => !string.IsNullOrEmpty(ad.ImagePath)); } }
    public IQueryable<TinyLink> TinyLinks { get { return db.TinyLinks; } }
    public IQueryable<PostCategory> PostCategories { get { return db.PostCategories.Where(r => r.SiteId == Site.Id); } }
    public IQueryable<PostImage> PostImages { get { return db.PostImages.Where(r => r.SiteId == Site.Id); } }
    public IQueryable<OutputCategory> OutputCategories { get { return db.OutputCategories.Where(r => r.SiteId == Site.Id); } }
    public IQueryable<OutputItem> OutputItems { get { return db.OutputItems.Where(r => r.SiteId == Site.Id); } }
    public void Add(object entity) { db.AddObject(GetEntitySetName(entity), entity); }

    string GetEntitySetName(object entity) {
      string typeName = entity.GetType().Name;
      switch (typeName) {
        case "PostCategory": return "PostCategories";
        case "OutputCategory": return "OutputCategories";
        default: return typeName + "s";
      }
    }

    public void Delete(object entity) { db.DeleteObject(entity); }

    public Site Site {
      get {
        if (site == null) {
          int siteId = int.Parse(ConfigurationManager.AppSettings["SiteId"]);
          site = db.Sites.Single(s => s.Id == siteId);
        }
        return site;
      }
    }


    public void SaveChanges() {
      db.SaveChanges();
    }

    public void Dispose() {
      db.Dispose();
    }
  }
}