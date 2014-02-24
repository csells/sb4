using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace sb4.Code {
  public class AmazonHelper {
    // e.g. http://www.amazon.com/exec/obidos/ASIN/9780321545619/chrissells
    internal static string GetAmazonLink(string isbn) {
      return string.Format("http://www.amazon.com/exec/obidos/ASIN/{0}/chrissells", isbn.Replace("-", ""));
    }
  }
}