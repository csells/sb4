using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace sb4 {
  public interface IValidate {
    IEnumerable<RuleViolation> GetRuleViolations();
  }

  public class RuleViolation {
    public RuleViolation(string property, string error) { Property = property; Error = error; }
    public string Property { get; private set; }
    public string Error { get; private set; }
  }
}
