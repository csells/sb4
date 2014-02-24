using System;
using System.Linq;
using System.Text;
using sb4;
using sb4.Code;

namespace sb4.ViewModels {
  public class WritingIndexViewModel : MasterViewModel {
    public WritingIndexViewModel(ISbDatabase db)
      : base(db) {
      OutputCategories = db.OutputCategories.OrderBy(c => c.Order);
    }

    public IQueryable<OutputCategory> OutputCategories { get; private set; }

    static string GetItemLink(OutputItem item) {
      if (string.IsNullOrEmpty(item.ISBN)) { return item.Link; }
      return AmazonHelper.GetAmazonLink(item.ISBN);
    }

    static void AppendNotEmptyWithComma(StringBuilder sb, string s) {
      if (!string.IsNullOrEmpty(s)) { sb.Append(", ").Append(s); }
    }

    public string GetItemHtml(OutputItem item) {
      var sb = new StringBuilder();
      var link = GetItemLink(item);
      if (!string.IsNullOrEmpty(link)) {
        sb.AppendFormat("<a href=\"{0}\">{1}</a>", link, item.Title);
      }
      else {
        sb.Append(item.Title);
      }

      AppendNotEmptyWithComma(sb, item.Authors);
      AppendNotEmptyWithComma(sb, item.Publisher);
      AppendNotEmptyWithComma(sb, item.Date == null ? null : item.Date.Value.ToShortDateString());
      AppendNotEmptyWithComma(sb, item.Notes);
      if (sb[sb.Length - 1] != '.') { sb.Append('.'); }

      return sb.ToString();
    }

  }
}
