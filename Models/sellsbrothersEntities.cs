using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data.Objects;
using System.Data;

namespace sb4.Models {
  public partial class sellsbrothersEntities {

    // TODO: right place to do this for POCO? ask Jeffders or dsimmons
    public override int SaveChanges(SaveOptions options) {
      Validate();
      return base.SaveChanges(options);
    }

    // TODO: proper way to hook this functionality up now that I'm using POCO?
    //partial void OnContextCreated() {
    //  this.SavingChanges += new EventHandler(this_SavingChanges);
    //}

    //protected void this_SavingChanges(object sender, EventArgs e) {
    //  Validate();
    //}

    private void Validate() {
      foreach (var entry in ObjectStateManager.GetObjectStateEntries(EntityState.Added | EntityState.Modified)) {
        IValidate validate = entry.Entity as IValidate;
        if (validate == null) { continue; }
        if (validate.GetRuleViolations().Any(dummy => true)) { throw new Exception("One or more rule violations."); }
      }
    }
  }

}