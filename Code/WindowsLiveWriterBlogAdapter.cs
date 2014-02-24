using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using ODataBlog;

namespace sb4.Code {
  public class WindowsLiveWriterBlogAdapter : IBlogAdapter {
    ISbDatabase db;

    public WindowsLiveWriterBlogAdapter() : this(null) { }

    public WindowsLiveWriterBlogAdapter(ISbDatabase db) {
      this.db = db ?? new sbdb();
    }

    public void AttachCategoriesToPost(AtomPost atomPost, IEnumerable<AtomCategory> atomCategories) {
      // Set the post's categories
      Post post = db.ActivePosts.Single(p => p.Id == atomPost.PostId);
      post.Categories = atomCategories.
        Select(c => c.Name).
        Aggregate("", (working, c) => working.Length == 0 ? c : working + "," + c);

      // Make sure all the categories are in the database
      var newCategories = atomCategories.
        Where(ac => !db.PostCategories.Any(pc => pc.Name.Equals(ac.Name, StringComparison.CurrentCultureIgnoreCase))).
        Select(ac => new PostCategory() { DisplayName = ac.Name, Name = ac.Name, Site = db.Site });
      foreach (var newCategory in newCategories) { db.Add(newCategory); }

      db.SaveChanges();
    }

    public void DeletePost(AtomPost atomPost) {
      Post post = db.GetPost(atomPost.PostId);
      post.IsActive = false;
      db.SaveChanges();
    }

    public IEnumerable<AtomCategory> GetCategories() {
      string categoryScheme = GetCategoryScheme();
      return db.PostCategories.Select(pc => new AtomCategory() { CategoryId = pc.Id, Name = pc.Name, Scheme = categoryScheme });
    }

    public IEnumerable<AtomCategory> GetCategoriesForPost(AtomPost atomPost) {
      Post post = db.ActivePosts.Single(p => p.Id == atomPost.PostId);
      IList<PostCategory> postCategories = db.PostCategories.ToList();

      return post.
        Categories.
        Split(',').
        Where(c => !string.IsNullOrWhiteSpace(c)).
        Select(c => new AtomCategory() {
          CategoryId = postCategories.Single(ac => ac.Name.Equals(c, StringComparison.CurrentCultureIgnoreCase)).Id,
          Name = c,
          Scheme = GetCategoryScheme()
        });
    }

    public string GetCategoryScheme() {
      return "http://sellsbrothers.com";
    }

    public byte[] GetPictureBytes(object item) {
      return ((PostImage)item).ImageData;
    }

    public string GetPictureInfoCollectionName() {
      return "PostImages";
    }

    public string GetPostCollectionName() {
      return "Posts";
    }

    public string GetPictureInfoTypeName() {
      return typeof(PostImage).FullName;
    }

    public string GetPictureMimeType(object item) {
      return ((PostImage)item).ContentType ?? "image";
    }

    public AtomPost GetPostById(int id) {
      Post post = db.GetPost(id);
      return new AtomPost() { Content = post.Content, PostId = post.Id, PublishDate = post.CreationDate, Title = post.Title };
    }

    public string GetServiceFileName() {
      return "wlw.svc";
    }

    public void UpdatePicture(object imageEntityWithIdOnly, AtomPictureInfo atomPictureInfo, byte[] content) {
      PostImage image = db.PostImages.Single(pi => pi.Id == ((PostImage)imageEntityWithIdOnly).Id);
      image.ContentType = atomPictureInfo.ContentType;
      image.ImageData = content;
      image.Site = db.Site;
      image.Size = atomPictureInfo.Size;
      db.SaveChanges();
    }

    public void UpdatePost(AtomPost p) {
      throw new NotImplementedException("WindowsLiveWriterBlogAdapter.UpdatePost");
    }

  }

}
