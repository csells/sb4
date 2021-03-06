﻿using System;
using System.Configuration;
using System.Diagnostics;
using System.Linq;
using System.Net.Mail;
using System.Reflection;
using System.Runtime.InteropServices;
using System.ServiceModel.Syndication;
using System.Text;
using System.Text.RegularExpressions;
using System.Web;
using System.Web.Mvc;
using sb4.Code;
using sb4.ViewModels;
using System.Xml;

namespace sb4.Controllers {
  public class PostsController : SbController {
    public PostsController() : base(new sbdb()) { }
    public PostsController(ISbDatabase db) : base(db) { }

    public ActionResult Index(string category, string format = "html", int page = 0) {
      switch (Request.HttpMethod.ToUpper()) {
        case "GET": return GetPosts(category, format, page);
        case "POST": return CreateAtomPost();
        default: return new SimpleActionResult() { StatusCode = 405, StatusDescription = "Unsupported HTTP Method" };
      }
    }

    ActionResult GetPosts(string category, string format, int page) {
      var vm = new PostIndexViewModel(db, category, page);

      switch (format.ToLower()) {
        case "atom":
        case "atom1":
        case "atom10":
          return new AtomActionResult(GetFeed(vm, "ATOM 1.0"));

        case "rss":
        case "rss2":
        case "rss20":
          return new RssActionResult(GetFeed(vm, "RSS 2.0"));

        case null:
        case "":
        case "html":
        case "xhtml":
          return View(vm);

        default:
          ViewData["format"] = format;
          return View("InvalidFormat", vm);
      }
    }

    static readonly DateTime minDate = new DateTime(1753, 1, 1); // SQL Server's min date
    static readonly string categoryScheme = "http://sellsbrothers.com"; // TODO: put the category scheme into the database

    public ActionResult AtomPub() {
      return new ServiceDocumentActionResult(GetServiceDocument());
    }

    public ActionResult AtomImages() {
      switch (Request.HttpMethod.ToUpper()) {
        case "POST": return CreateAtomImage();
        default: return new SimpleActionResult() { StatusCode = 405, StatusDescription = "Unsupported HTTP Method" };
      }
    }

    public ActionResult AtomImage(int id) {
      switch (Request.HttpMethod.ToUpper()) {
        case "GET": return GetAtomImage(id);
        default: return new SimpleActionResult() { StatusCode = 405, StatusDescription = "Unsupported HTTP Method" };
      }
    }

    public ActionResult Image(int id) {
      switch (Request.HttpMethod.ToUpper()) {
        case "GET": return GetImage(id);
        default: return new SimpleActionResult() { StatusCode = 405, StatusDescription = "Unsupported HTTP Method" };
      }
    }

    ActionResult CreateAtomPost() {
      if (!BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdmin()) { return null; }

      // Get post data
      SyndicationItem entry = null;
      using (var reader = XmlReader.Create(Request.InputStream)) {
        entry = SyndicationItem.Load(reader);
      }

      // Create post
      var post = new Post() {
        Categories = entry.Categories.Aggregate("", (cats, cat) => cats.Length == 0 ? cat.Name : cats + "," + cat.Name),
        Content = ((TextSyndicationContent)entry.Content).Text,
        CreationDate = entry.PublishDate < minDate ? DateTime.Now : entry.PublishDate.DateTime,
        Author = entry.Authors.Count > 0 ? entry.Authors[0].Name : null,
        Email = entry.Authors.Count > 0 ? entry.Authors[0].Email : null,
        IsActive = true,
        Title = entry.Title.Text,
        UuidString = Guid.NewGuid().ToString(),
      };

      db.Add(post);
      db.SaveChanges();

      // Return the updated post
      var postLink = GetAtomPostLink(post.Id);
      entry.Id = postLink;
      entry.Authors.Add(new SyndicationPerson() { Name = db.Site.ContactName, Email = db.Site.ContactEmail });

      var sb = new StringBuilder();
      // OMG! WLW crashes if it gets back an XML declaration!
      using (var writer = XmlWriter.Create(sb, new XmlWriterSettings() { OmitXmlDeclaration = true })) { entry.SaveAsAtom10(writer); }
      var result = new SimpleActionResult() { ResponseOut = sb.ToString(), StatusCode = 201, StatusDescription = "Created" };
      result.Headers.Add("Location", postLink);
      return result;
    }

    // e.g.
    // GET: /Posts/Details/5
    public ActionResult Details(int id) {
      var vm = new PostDetailsViewModel(db, id, Request.Url);
      if (vm.Post == null) {
        ViewData["id"] = id;
        return View("NotFound", vm);
      }

      return View(vm);
    }

    // e.g.
    // GET: /Posts/AtomDetails/5
    public ActionResult AtomDetails(int id) {
      switch (Request.HttpMethod.ToUpper()) {
        case "GET": return GetAtomPost(id);
        case "PUT": return UpdateAtomPost(id);
        case "DELETE": return DeleteAtomPost(id);
        default: return new SimpleActionResult() { StatusCode = 405, StatusDescription = "Unsupported HTTP Method" };
      }
    }

    ActionResult GetAtomPost(int id) {
      var vm = new PostDetailsViewModel(db, id, Request.Url);
      if (vm.Post == null) {
        return new SimpleActionResult() { StatusCode = 404, StatusDescription = "Resource Not Found" };
      }

      var entry = GetEntry(vm.Post);
      var sb = new StringBuilder();
      // OMG! WLW crashes if it gets back an XML declaration!
      using (var writer = XmlWriter.Create(sb, new XmlWriterSettings() { OmitXmlDeclaration = true })) { entry.SaveAsAtom10(writer); }
      return new SimpleActionResult() { ResponseOut = sb.ToString(), ContentType = "application/xml" };
    }

    ActionResult UpdateAtomPost(int id) {
      if (!BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdmin()) { return null; }

      var vm = new PostDetailsViewModel(db, id, Request.Url);
      if (vm.Post == null) {
        return new SimpleActionResult() { StatusCode = 404, StatusDescription = "Resource Not Found" };
      }

      // Get post data
      SyndicationItem entry = null;
      using (var reader = XmlReader.Create(Request.InputStream)) {
        entry = SyndicationItem.Load(reader);
      }

      // Update post
      var post = vm.Post;
      post.Categories = entry.Categories.Aggregate("", (cats, cat) => cats.Length == 0 ? cat.Name : cats + "," + cat.Name);
      post.Content = ((TextSyndicationContent)entry.Content).Text;
      post.CreationDate = entry.PublishDate < minDate ? DateTime.Now : entry.PublishDate.DateTime;
      post.Author = entry.Authors.Count > 0 ? entry.Authors[0].Name : null;
      post.IsActive = true;
      post.Title = entry.Title.Text;
      db.SaveChanges();

      return new SimpleActionResult();
    }

    ActionResult DeleteAtomPost(int id) {
      if (!BasicAuthSingleAdminUserModule.ForceSslAndBasicAuthAsAdmin()) { return null; }

      var vm = new PostDetailsViewModel(db, id, Request.Url);
      if (vm.Post == null) {
        return new SimpleActionResult() { StatusCode = 404, StatusDescription = "Resource Not Found" };
      }

      vm.Post.IsActive = false;
      db.SaveChanges();

      return new SimpleActionResult();
    }

    ActionResult CreateAtomImage() {
      // Create image
      var image = new PostImage() {
        ContentType = Request.ContentType,
        ImageData = Request.BinaryRead(Request.ContentLength),
        Site = db.Site,
        Size = Request.ContentLength,
      };
      db.Add(image);
      db.SaveChanges();

      // Return image URL
      var result = new SimpleActionResult() { StatusCode = 201, StatusDescription = "Created" };
      result.Headers.Add("Location", GetAtomImageLink(image.Id));

      return result;
    }

    ActionResult GetImage(int id) {
      var image = db.PostImages.SingleOrDefault(i => i.Id == id);
      if (image == null) { return new SimpleActionResult() { StatusCode = 404, StatusDescription = "Resource Not Found" }; }
      return new SimpleActionResult() { ContentType = image.ContentType, ResponseBytes = image.ImageData };
    }

    ActionResult GetAtomImage(int id) {
      var image = db.PostImages.SingleOrDefault(i=>i.Id == id);
      if (image == null) { return new SimpleActionResult() { StatusCode = 404, StatusDescription = "Resource Not Found" }; }

      var entry = GetEntry(image);
      var sb = new StringBuilder();
      // OMG! WLW crashes if it gets back an XML declaration!
      using (var writer = XmlWriter.Create(sb, new XmlWriterSettings() { OmitXmlDeclaration = true })) { entry.SaveAsAtom10(writer); }
      return new SimpleActionResult() { ResponseOut = sb.ToString(), ContentType = "application/xml" };
    }

    // Helpers
    string GetIdLink(string action, int id) {
      var link = HttpContext.Request.Url.GetLeftPart(UriPartial.Authority) + Url.Action(action, new { id = id });
      //link = link.Replace("localhost:81", "localhost:8080");
      link = link.Replace("https://", "http://"); // fix the issue with images going in with "https" links
      return link;
    }

    internal string GetPostLink(int id) {
      return GetIdLink("Details", id);
    }

    internal string GetAtomPostLink(int id) {
      return GetIdLink("AtomDetails", id);
    }

    internal string GetImageLink(int id) {
      return GetIdLink("Image", id);
    }

    internal string GetAtomImageLink(int id) {
      return GetIdLink("AtomImage", id);
    }

    ServiceDocument GetServiceDocument() {
      var doc = new ServiceDocument(new Workspace[] {
        new Workspace("Default", new ResourceCollectionInfo[] {
          new ResourceCollectionInfo("Posts", new Uri(Url.Action("Index", new { format = "atom10" }), UriKind.Relative)) { Categories = { new InlineCategoriesDocument() { IsFixed = false, Scheme = categoryScheme } } },
          new ResourceCollectionInfo(new TextSyndicationContent("Images"), new Uri(Url.Action("AtomImages"), UriKind.Relative), null, new string[] { "image/*" } ),
        }),
      });

      var categories = ((InlineCategoriesDocument)doc.Workspaces[0].Collections[0].Categories[0]).Categories;
      foreach (var cat in db.PostCategories) {
        categories.Add(new SyndicationCategory(cat.Name, null, cat.DisplayName));
      }

      return doc;
    }

    internal SyndicationFeed GetFeed(PostIndexViewModel vm, string format = null) {
      var posts = vm.Posts.ToList();
      var feed = new SyndicationFeed() {
        Authors = { new SyndicationPerson(vm.ContactEmail, vm.ContactName, Request.Url.GetLeftPart(UriPartial.Authority)) },
        ImageUrl = new Uri(string.Format("{0}/content/images/sellsbrothers_feed_logo.jpg", Request.Url.GetLeftPart(UriPartial.Authority))),
        Copyright = new TextSyndicationContent(vm.CopyrightNotice),
        Description = new TextSyndicationContent(vm.About),
        Id = string.Format("uuid:{0}", ((GuidAttribute)Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(GuidAttribute), false)[0]).Value.ToString()),
        Language = "en-us",
        LastUpdatedTime = posts.Count == 0 ? DateTime.Now : posts.First().CreationDate,
        Title = new TextSyndicationContent(vm.GetFeedTitle(format)),
        Items = posts.Select(p => GetEntry(p)),
      };

      foreach (var cat in vm.PostCategories) {
        feed.Categories.Add(new SyndicationCategory(cat.Name, categoryScheme, cat.DisplayName));
      }

      return feed;
    }

    SyndicationItem GetEntry(Post post) {
      string authorEmail = post.Email;
      string authorName = post.Author;
      if (string.IsNullOrWhiteSpace(authorEmail) && string.IsNullOrWhiteSpace(authorName)) {
        authorEmail = db.Site.ContactEmail;
        authorName = db.Site.ContactName;
      }

      var entry = new SyndicationItem() {
        Content = new TextSyndicationContent(post.Content, TextSyndicationContentKind.Html),
        Id = GetPostLink(post.Id),
        LastUpdatedTime = post.CreationDate,
        Links = {
          new SyndicationLink(new Uri(GetAtomPostLink(post.Id)), "edit", null, null, 0),
          new SyndicationLink(new Uri(GetPostLink(post.Id)), "alternate", null, "text/html", 0),
        },
        PublishDate = post.CreationDate,
        Title = new TextSyndicationContent(post.Title),
        Authors = { new SyndicationPerson(authorEmail, authorName, null) },
      };

      // Split each item category with embedded commas
      if (post.Categories != null && post.Categories.Contains(',')) {
        foreach (var category in post.Categories.Split(',')) {
          entry.Categories.Add(new SyndicationCategory(category));
        }
      }

      return entry;
    }

    SyndicationItem GetEntry(PostImage image) {
      var entry = new SyndicationItem() {
        Content = new UrlSyndicationContent(new Uri(GetImageLink(image.Id)), image.ContentType),
        Id = GetAtomImageLink(image.Id),
        Links = {
          new SyndicationLink(new Uri(GetAtomImageLink(image.Id)), "edit", null, null, 0),
          new SyndicationLink(new Uri(GetImageLink(image.Id)), "edit-media", null, image.ContentType, 0),
        },
      };

      return entry;
    }

  }
}
