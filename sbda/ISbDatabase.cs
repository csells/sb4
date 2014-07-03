using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace sb4 {
  public interface ISbDatabase {
    IQueryable<Badge> Badges { get; }
    IQueryable<LandingZone> LandingZones { get; }
    Post GetPost(int id);
    IQueryable<Post> ActivePosts { get; }
    Site Site { get; }
    IQueryable<Ad> TextAds { get; }
    IQueryable<Ad> BannerAds { get; }
    IQueryable<TinyLink> TinyLinks { get; }
    IQueryable<PostCategory> PostCategories { get; }
    IQueryable<PostImage> PostImages { get; }
    IQueryable<OutputCategory> OutputCategories { get; }
    IQueryable<OutputItem> OutputItems { get; }
    void Add(object entity);
    void Delete(object entity);
    void SaveChanges();
  }
}
