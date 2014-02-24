// From http://addinit.com/?q=node/62

using System;
using System.Web.Mvc;
using System.Configuration;

namespace Recaptcha.Mvc
{
    /// <summary>
    /// Renders a reCAPTCHA control.
    /// </summary>
    public static class RecaptchaHtmlHelper
    {
        /// <summary>
        /// Renders a reCAPTCHA
        /// </summary>        
        /// <param name="id">containing element ID</param>        
        public static string CreateRecaptcha(this HtmlHelper helper, string id)
        {
            return CreateRecaptcha(
                helper,
                id,
                ConfigurationManager.AppSettings["RecaptchaPrivateKey"],
                ConfigurationManager.AppSettings["RecaptchaPublicKey"],
                null);
        }

        /// <summary>
        /// Renders a reCAPTCHA
        /// </summary>        
        /// <param name="id">containing element ID</param>        
        /// <param name="theme">reCAPTCHA theme</param>     
        public static string CreateRecaptcha(this HtmlHelper helper, string id, string theme)
        {
            return CreateRecaptcha(
                helper,
                id,
                ConfigurationManager.AppSettings["RecaptchaPrivateKey"],
                ConfigurationManager.AppSettings["RecaptchaPublicKey"],
                theme);
        }

        /// <summary>
        /// Renders a reCAPTCHA
        /// </summary>        
        /// <param name="id">containing element ID</param>
        /// <param name="privateKey">your reCAPTCHA account's private key</param>
        /// <param name="publicKey">your reCAPTCHA account's public key</param>        
        public static string CreateRecaptcha(this HtmlHelper helper, string id, string privateKey, string publicKey)
        {
            return CreateRecaptcha(
                helper,
                id,
                privateKey,
                publicKey,
                null);
        }

        /// <summary>
        /// Renders a reCAPTCHA
        /// </summary>        
        /// <param name="id">containing element ID</param>
        /// <param name="privateKey">your reCAPTCHA account's private key</param>
        /// <param name="publicKey">your reCAPTCHA account's public key</param>
        /// <param name="theme">reCAPTCHA theme</param>        
        public static string CreateRecaptcha(this HtmlHelper helper, string id, string privateKey, string publicKey, string theme)
        {
            if (string.IsNullOrEmpty(privateKey) || string.IsNullOrEmpty(publicKey))
                throw new ApplicationException("reCAPTCHA HTML Helper needs to be configured with a public and private key.");
            
            var control = new RecaptchaControl();
            control.ID = id;
            control.PrivateKey = privateKey;
            control.PublicKey = publicKey;
            control.Theme = theme;

            System.IO.StringWriter sw = new System.IO.StringWriter();
            var htmlwriter = new System.Web.UI.HtmlTextWriter(sw);
            control.RenderControl(htmlwriter);
            return sw.ToString();            
        }
    }
}