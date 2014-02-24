// From http://addinit.com/?q=node/62

using System;
using System.Web.Mvc;
using System.Configuration;

namespace Recaptcha.Mvc
{
    /// <summary>
    /// ActionFilter that allows for checking of reCAPTCHA.
    /// </summary>
    public class RecaptchaFilterAttribute : ActionFilterAttribute
    {
        public RecaptchaFilterAttribute()
        {
            this.privateKey = System.Configuration.ConfigurationManager.AppSettings["RecaptchaPrivateKey"];
            this.ignoreOnMobile = false;
            this.ignore = null;
        }

        #region Private Fields 

        private string privateKey;
        private bool ignoreOnMobile;
        private Func<bool> ignore;

        #endregion Private Fields

        #region Public Properties

        /// <summary>
        /// Your private key for reCAPTCHA.
        /// </summary>
        public string PrivateKey
        {
            get { return this.privateKey; }
            set { this.privateKey = value; }
        }
        
        /// <summary>
        /// Ignores reCAPTCHA checking when browser is detected as a mobile browser.
        /// </summary>
        public bool IgnoreOnMobile
        {
            get { return this.ignoreOnMobile; }
            set { this.ignoreOnMobile = value; }
        }

        /// <summary>
        /// Delegate for checking for reCAPTCHA ignore conditions.  Allows you to 
        /// use custom logic to ignore reCAPTCHA.
        /// </summary>
        public Func<bool> Ignore
        {
            get { return this.ignore; }
            set { this.ignore = value; }
        }

        #endregion Public Properties

        #region Overrides

        public override void OnActionExecuting(ActionExecutingContext filterContext)
        {    
            // check for mobile browser and ignoreonmobile property -> return if both conditions are true
            if (IgnoreOnMobile && filterContext.RequestContext.HttpContext.Request.Browser.IsMobileDevice)
                return;

            // check for ignore delegate
            if (this.Ignore != null && this.Ignore())
                return;

            // ensure we have required parameters
            if (string.IsNullOrEmpty(PrivateKey))
                throw new ApplicationException("reCAPTCHA Action Filter needs to be configured with a private key.");
            
            // build the validation request
            var validator = new RecaptchaValidator();            
            validator.PrivateKey = this.privateKey;
            validator.Challenge = filterContext.HttpContext.Request["recaptcha_challenge_field"];
            validator.Response = filterContext.HttpContext.Request["recaptcha_response_field"];
            validator.RemoteIP = filterContext.HttpContext.Request.UserHostAddress;

            // validate the request
            RecaptchaResponse response = validator.Validate();

            // get the controller so we can access the model state
            var controller = filterContext.Controller as Controller;
            if (controller == null)
                throw new ApplicationException("Controller must be of type" + typeof(Controller).ToString());

            // add the model state error
            if (!response.IsValid)
                controller.ModelState.AddModelError("Recaptcha", "Recaptcha was not valid, please try again.");
        }

        #endregion Overrides
    }
}