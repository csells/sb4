using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Text.RegularExpressions;

namespace sb4 {
  public partial class Comment : IValidate {
    //static Regex lenientEmailRegex = new Regex(@"(?<userinfo>\w+([-+.]\w+)*)@(?<host>\w+([-.]\w+)*\.\w+([-.]\w+)*)");
    static Regex strictEmailRegex = new Regex(@"^(([^<>()[\]\\.,;:\s@\""]+(\.[^<>()[\]\\.,;:\s@\""]+)*)|(\"".+\""))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$");

    public IEnumerable<RuleViolation> GetRuleViolations() {
      if (string.IsNullOrEmpty(this.Author)) { yield return new RuleViolation("Author", "Invalid Name: cannot be empty"); }
      if (!string.IsNullOrEmpty(this.Email) && !IsValidEmail(this.Email)) { yield return new RuleViolation("Email", "Invalid Email: bad format"); }
      if (ShouldEmailNotifications && string.IsNullOrEmpty(this.Email)) { yield return new RuleViolation("Email", "Invalid Email: need email to send notifications"); }
      if (string.IsNullOrEmpty(this.Content)) { yield return new RuleViolation("Content", "Invalid Comment: cannot be empty"); }
    }

    bool IsValidEmail(string email) {
      return strictEmailRegex.IsMatch(email);
    }

  }

}