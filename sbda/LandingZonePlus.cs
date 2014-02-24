using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Text.RegularExpressions;

namespace sb4 {
  public partial class LandingZone {
    public string HeaderImageExtPath {
      get {
        string p = HeaderImagePath;
        string ext = System.IO.Path.GetExtension(p);
        return p.Substring(0, p.Length - ext.Length) + "_ext" + ext;
      }
    }
  }
}