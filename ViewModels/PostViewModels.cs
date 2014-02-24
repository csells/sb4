using System;
using System.Collections.Generic;
using System.Linq;
using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;

namespace sb4.ViewModels {
  public class PostIndexViewModel : MasterViewModel {
    public PostIndexViewModel(ISbDatabase db, string category = "", int page = 0)
      : base(db) {
      CurrentCategory = category ?? "";

      Title = db.Site.Title;
      About = db.Site.BriefAbout;
      PageSize = db.Site.PageSize;
      var categoryPosts = FilterPostsByCategory(db.ActivePosts, CurrentCategory);
      int totalPostCount = categoryPosts.Count();
      Page = Math.Min(Math.Max(page, 0), (int)Math.Floor((double)totalPostCount / (double)PageSize));
      Posts = FilterPostsByPage(categoryPosts, PageSize, Page);

      var cat = PostCategories.FirstOrDefault(c => string.Compare(c.Name, CurrentCategory, true) == 0);
      if (cat != null) { CurrentCategoryName = cat.DisplayName; }
      else { CurrentCategoryName = CurrentCategory; }

      NewerPostCount = Page * PageSize;
      OlderPostCount = Math.Max(0, totalPostCount - (Page + 1) * PageSize);
    }

    public string Title { get; private set; }
    public string About { get; private set; }
    public IQueryable<Post> Posts { get; private set; }
    public string CurrentCategory { get; private set; }
    public string CurrentCategoryName { get; private set; }
    public int Page { get; private set; }
    public int PageSize { get; private set; }
    public int OlderPostCount { get; private set; }
    public int NewerPostCount { get; private set; }

    static IQueryable<Post> FilterPostsByCategory(IQueryable<Post> posts, string category) {
      if (string.IsNullOrEmpty(category)) { return posts; }
      return posts.Where(p => p.Categories.Contains(category));
    }

    static IQueryable<Post> FilterPostsByPage(IQueryable<Post> posts, int pageSize, int page) {
      return posts.OrderByDescending(p => p.CreationDate).Skip(pageSize * page).Take(pageSize);
    }

    public string GetFeedTitle(string format) {
      var titleBuilder = new StringBuilder(Title);
      if (!string.IsNullOrEmpty(CurrentCategory)) { titleBuilder.AppendFormat(" for category '{0}'", CurrentCategory); }
      if (!string.IsNullOrEmpty(format)) { titleBuilder.AppendFormat(" via {0}", format); }
      return titleBuilder.ToString();
    }

    public string GetFeedLink(Uri requestUrl, string format) {
      // http://localhost:10749/odata.svc/Posts?$orderby=CreationDate desc&$top=25&filter=substringof('fun', tolower(Category))
      var linkBuilder = new StringBuilder(requestUrl.GetLeftPart(UriPartial.Authority));
      switch (format.ToLower()) {
        case "odata":
        case "odata2":
        case "odata20":
          linkBuilder.Append("/odata.svc/Posts?$orderby=CreationDate desc");
          if (!string.IsNullOrEmpty(CurrentCategory)) { linkBuilder.AppendFormat("&$filter=substringof('{0}', tolower(Category))", CurrentCategory); }
          break;

        default:
          linkBuilder.AppendFormat("/posts?format={0}", format);
          if (!string.IsNullOrEmpty(CurrentCategory)) { linkBuilder.AppendFormat("&category={0}", CurrentCategory); }
          break;
      }

      return linkBuilder.ToString();
    }
  }

  public class PostDetailsViewModel : MasterViewModel {
    public PostDetailsViewModel(ISbDatabase db, int id, Uri requestUrl)
      : base(db) {
      Post = db.ActivePosts.SingleOrDefault(p => p.Id == id);
      RequestUrl = requestUrl;
      AllPostCategories = db.PostCategories;
    }

    public Post Post { get; private set; }
    Uri RequestUrl { get; set; }
    public IEnumerable<PostCategory> AllPostCategories { get; private set; }

    static Regex spacesRegex = new Regex(@"(?<=^ *) ", RegexOptions.Multiline);

    public string FormatComment(string comment) {
      return spacesRegex.Replace(comment, "&nbsp;").Replace("\r", "").Replace("\n", "<br \\>\r\n");
    }

    public string GetCommentFeedLink() {
      return Controllers.PostsController.GetCommentRssLink(RequestUrl, Post.Id);
    }

  }
}