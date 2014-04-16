using Recaptcha.Design;
using System;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Configuration;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace Recaptcha
{
	[Designer(typeof(RecaptchaControlDesigner))]
	[ToolboxData("<{0}:RecaptchaControl runat=\"server\" />")]
	public class RecaptchaControl : WebControl, IValidator
	{
		private const string RECAPTCHA_CHALLENGE_FIELD = "recaptcha_challenge_field";

		private const string RECAPTCHA_RESPONSE_FIELD = "recaptcha_response_field";

		private const string RECAPTCHA_SECURE_HOST = "https://api-secure.recaptcha.net";

		private const string RECAPTCHA_HOST = "http://api.recaptcha.net";

		private RecaptchaResponse recaptchaResponse;

		private string publicKey;

		private string privateKey;

		private string theme;

		private string customThemeWidget;

		private string errorMessage;

		private bool skipRecaptcha;

		private bool allowMultipleInstances;

		private bool overrideSecureMode;

		[Category("Settings")]
		[DefaultValue(false)]
		[Description("Set this to true to enable multiple reCAPTCHA on a single page. There may be complication between controls when this is enabled.")]
		public bool AllowMultipleInstances
		{
			get
			{
				return this.allowMultipleInstances;
			}
			set
			{
				this.allowMultipleInstances = value;
			}
		}

		[Category("Appearence")]
		[DefaultValue(null)]
		[Description("When using custom theming, this is a div element which contains the widget. ")]
		public string CustomThemeWidget
		{
			get
			{
				return this.customThemeWidget;
			}
			set
			{
				this.customThemeWidget = value;
			}
		}

		[DefaultValue("The verification words are incorrect.")]
		[Localizable(true)]
		public string ErrorMessage
		{
			get
			{
				if (this.errorMessage == null)
				{
					return "The verification words are incorrect.";
				}
				return this.errorMessage;
			}
			set
			{
				this.errorMessage = value;
			}
		}

		[Browsable(false)]
		public bool IsValid
		{
			get
			{
				if (!this.Page.IsPostBack || !this.Visible || !this.Enabled || this.skipRecaptcha)
				{
					return true;
				}
				if (this.recaptchaResponse == null)
				{
					this.Validate();
				}
				if (this.recaptchaResponse == null)
				{
					return false;
				}
				return this.recaptchaResponse.IsValid;
			}
			set
			{
				throw new NotImplementedException("This setter is not implemented.");
			}
		}

		[Category("Settings")]
		[DefaultValue(false)]
		[Description("Set this to true to override reCAPTCHA usage of Secure API.")]
		public bool OverrideSecureMode
		{
			get
			{
				return this.overrideSecureMode;
			}
			set
			{
				this.overrideSecureMode = value;
			}
		}

		[Category("Settings")]
		[Description("The private key from admin.recaptcha.net. Can also be set using RecaptchaPrivateKey in AppSettings.")]
		public string PrivateKey
		{
			get
			{
				return this.privateKey;
			}
			set
			{
				this.privateKey = value;
			}
		}

		[Category("Settings")]
		[Description("The public key from admin.recaptcha.net. Can also be set using RecaptchaPublicKey in AppSettings.")]
		public string PublicKey
		{
			get
			{
				return this.publicKey;
			}
			set
			{
				this.publicKey = value;
			}
		}

		[Category("Settings")]
		[DefaultValue(false)]
		[Description("Set this to true to stop reCAPTCHA validation. Useful for testing platform. Can also be set using RecaptchaSkipValidation in AppSettings.")]
		public bool SkipRecaptcha
		{
			get
			{
				return this.skipRecaptcha;
			}
			set
			{
				this.skipRecaptcha = value;
			}
		}

		[Category("Appearence")]
		[DefaultValue("red")]
		[Description("The theme for the reCAPTCHA control. Currently supported values are red, blackglass, white, and clean")]
		public string Theme
		{
			get
			{
				return this.theme;
			}
			set
			{
				this.theme = value;
			}
		}

		public RecaptchaControl()
		{
			this.publicKey = ConfigurationManager.AppSettings["RecaptchaPublicKey"];
			this.privateKey = ConfigurationManager.AppSettings["RecaptchaPrivateKey"];
			if (!bool.TryParse(ConfigurationManager.AppSettings["RecaptchaSkipValidation"], out this.skipRecaptcha))
			{
				this.skipRecaptcha = false;
			}
		}

		private bool CheckIfRecaptchaExists()
		{
			bool flag;
			IEnumerator enumerator = this.Page.Validators.GetEnumerator();
			try
			{
				while (enumerator.MoveNext())
				{
					if (!(enumerator.Current is RecaptchaControl))
					{
						continue;
					}
					flag = true;
					return flag;
				}
				return false;
			}
			finally
			{
				IDisposable disposable = enumerator as IDisposable;
				if (disposable != null)
				{
					disposable.Dispose();
				}
			}
		}

		private string GenerateChallengeUrl(bool noScript)
		{
			StringBuilder stringBuilder = new StringBuilder();
			stringBuilder.Append((this.Context.Request.IsSecureConnection || this.overrideSecureMode ? "https://api-secure.recaptcha.net" : "http://api.recaptcha.net"));
			stringBuilder.Append((noScript ? "/noscript?" : "/challenge?"));
			stringBuilder.AppendFormat("k={0}", this.PublicKey);
			if (this.recaptchaResponse != null && this.recaptchaResponse.ErrorCode != string.Empty)
			{
				stringBuilder.AppendFormat("&error={0}", this.recaptchaResponse.ErrorCode);
			}
			return stringBuilder.ToString();
		}

		protected override void OnInit(EventArgs e)
		{
			base.OnInit(e);
			if (string.IsNullOrEmpty(this.PublicKey) || string.IsNullOrEmpty(this.PrivateKey))
			{
				throw new ApplicationException("reCAPTCHA needs to be configured with a public & private key.");
			}
			if (this.allowMultipleInstances || !this.CheckIfRecaptchaExists())
			{
				this.Page.Validators.Add(this);
			}
		}

		protected override void Render(HtmlTextWriter writer)
		{
			if (this.skipRecaptcha)
			{
				writer.WriteLine("reCAPTCHA validation is skipped. Set SkipRecaptcha property to false to enable validation.");
				return;
			}
			this.RenderContents(writer);
		}

		protected override void RenderContents(HtmlTextWriter output)
		{
			output.AddAttribute(HtmlTextWriterAttribute.Type, "text/javascript");
			output.RenderBeginTag(HtmlTextWriterTag.Script);
			HtmlTextWriter indent = output;
			indent.Indent = indent.Indent + 1;
			output.WriteLine("var RecaptchaOptions = {");
			HtmlTextWriter htmlTextWriter = output;
			htmlTextWriter.Indent = htmlTextWriter.Indent + 1;
			output.WriteLine("theme : '{0}',", this.theme ?? string.Empty);
			if (this.customThemeWidget != null)
			{
				output.WriteLine("custom_theme_widget : '{0}',", this.customThemeWidget);
			}
			output.WriteLine("tabindex : {0}", this.TabIndex);
			HtmlTextWriter indent1 = output;
			indent1.Indent = indent1.Indent - 1;
			output.WriteLine("};");
			HtmlTextWriter htmlTextWriter1 = output;
			htmlTextWriter1.Indent = htmlTextWriter1.Indent - 1;
			output.RenderEndTag();
			output.AddAttribute(HtmlTextWriterAttribute.Type, "text/javascript");
			output.AddAttribute(HtmlTextWriterAttribute.Src, this.GenerateChallengeUrl(false), false);
			output.RenderBeginTag(HtmlTextWriterTag.Script);
			output.RenderEndTag();
			output.RenderBeginTag(HtmlTextWriterTag.Noscript);
			HtmlTextWriter indent2 = output;
			indent2.Indent = indent2.Indent + 1;
			output.AddAttribute(HtmlTextWriterAttribute.Src, this.GenerateChallengeUrl(true), false);
			output.AddAttribute(HtmlTextWriterAttribute.Width, "500");
			output.AddAttribute(HtmlTextWriterAttribute.Height, "300");
			output.AddAttribute("frameborder", "0");
			output.RenderBeginTag(HtmlTextWriterTag.Iframe);
			output.RenderEndTag();
			output.WriteBreak();
			output.AddAttribute(HtmlTextWriterAttribute.Name, "recaptcha_challenge_field");
			output.AddAttribute(HtmlTextWriterAttribute.Rows, "3");
			output.AddAttribute(HtmlTextWriterAttribute.Cols, "40");
			output.RenderBeginTag(HtmlTextWriterTag.Textarea);
			output.RenderEndTag();
			output.AddAttribute(HtmlTextWriterAttribute.Name, "recaptcha_response_field");
			output.AddAttribute(HtmlTextWriterAttribute.Value, "manual_challenge");
			output.AddAttribute(HtmlTextWriterAttribute.Type, "hidden");
			output.RenderBeginTag(HtmlTextWriterTag.Input);
			output.RenderEndTag();
			HtmlTextWriter htmlTextWriter2 = output;
			htmlTextWriter2.Indent = htmlTextWriter2.Indent - 1;
			output.RenderEndTag();
		}

		public void Validate()
		{
			if (this.skipRecaptcha)
			{
				this.recaptchaResponse = RecaptchaResponse.Valid;
			}
			if (this.recaptchaResponse == null && this.Visible && this.Enabled)
			{
				RecaptchaValidator recaptchaValidator = new RecaptchaValidator()
				{
					PrivateKey = this.PrivateKey,
					RemoteIP = this.Page.Request.UserHostAddress,
					Challenge = this.Context.Request.Form["recaptcha_challenge_field"],
					Response = this.Context.Request.Form["recaptcha_response_field"]
				};
				try
				{
					this.recaptchaResponse = recaptchaValidator.Validate();
				}
				catch (ArgumentNullException argumentNullException1)
				{
					ArgumentNullException argumentNullException = argumentNullException1;
					this.recaptchaResponse = null;
					this.errorMessage = argumentNullException.Message;
				}
			}
		}
	}
}