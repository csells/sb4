using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Text.RegularExpressions;

namespace sb4 {
  public partial class Post {

    // Map the comma-delimited list of category names to a list of PostCategory objects so we can expose the DisplayName
    public IEnumerable<PostCategory> GetPostCategories(IEnumerable<PostCategory> allPostCategories) {
      // Just return known categories based on the comma-delimited list of categories
      if (string.IsNullOrWhiteSpace(Categories)) { return new PostCategory[0]; }
      string[] thisPostCategoryNames = Categories.Split(',').Where(n=>!string.IsNullOrWhiteSpace(n)).ToArray();
      return allPostCategories.Where(c => thisPostCategoryNames.Any(n => n.Equals(c.Name, StringComparison.CurrentCultureIgnoreCase)));


      // Here we're crufting up a new PostCategory for any that are used that don't exist in the database
      // TODO: crufting up non-existent categories for random strings doesn't work, as if the category isn't around, it won't be in the route map
      // TODO: what if I add a category and the route map isn't reloaded?!
      //var site = allPostCategories[0].Site;
      //return Categories.
      //  Split(',').
      //  Where(n=>!string.IsNullOrWhiteSpace(n)).
      //  Select(n => allPostCategories.FirstOrDefault(c => c.Name.Equals(n, StringComparison.CurrentCultureIgnoreCase)) ?? new PostCategory() { Name = n, DisplayName = n, Site = site });
    }

  }

}