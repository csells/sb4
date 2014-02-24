using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Web;
using System.Web.Services;

namespace SellsBrothers.excuses {

  [WebService(Namespace="http://sellsbrothers.com/excuses/")]
  public class excuses : System.Web.Services.WebService {
    public excuses() {
      //CODEGEN: This call is required by the ASP.NET Web Services Designer
      InitializeComponent();
    }

    #region Component Designer generated code
		
    //Required by the Web Services Designer 
    private IContainer components = null;
				
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent() {
    }

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose( bool disposing ) {
      if(disposing && components != null) {
        components.Dispose();
      }
      base.Dispose(disposing);		
    }
		
    #endregion

    [WebMethod]
    public string GetExcuse() {
      string[] excuses = {
                           "Jury Duty",
                           "Abducted by Aliens",
                           "Car Trouble",
                           "Traffic Was Bad",
                           "Mexican Food",
                           "Full Moon",
                           "Huh?",
                           "Amnesia",
                           "No Hablo Ingleses",
                           "Oprah",
                           "I've Got a Headache",
                           "24-Hour Flu",
                           "What Memo?",
                           "It's In the Mail",
                           "My Fish Died",
                           "I Was Mugged",
                           "The Voices Told Me To",
                           "My Dog Ate It",
                           "Kryptonite",
                           "It's Not My Job",
      };

      return excuses[(new Random()).Next(excuses.Length)];
    }

  }

}



