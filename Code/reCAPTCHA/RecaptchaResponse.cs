using System;

namespace Recaptcha
{
	public class RecaptchaResponse
	{
		public readonly static RecaptchaResponse Valid;

		public readonly static RecaptchaResponse InvalidSolution;

		public readonly static RecaptchaResponse RecaptchaNotReachable;

		private bool isValid;

		private string errorCode;

		public string ErrorCode
		{
			get
			{
				return this.errorCode;
			}
		}

		public bool IsValid
		{
			get
			{
				return this.isValid;
			}
		}

		static RecaptchaResponse()
		{
			RecaptchaResponse.Valid = new RecaptchaResponse(true, string.Empty);
			RecaptchaResponse.InvalidSolution = new RecaptchaResponse(false, "incorrect-captcha-sol");
			RecaptchaResponse.RecaptchaNotReachable = new RecaptchaResponse(false, "recaptcha-not-reachable");
		}

		internal RecaptchaResponse(bool isValid, string errorCode)
		{
			this.isValid = isValid;
			this.errorCode = errorCode;
		}

		public override bool Equals(object obj)
		{
			RecaptchaResponse recaptchaResponse = (RecaptchaResponse)obj;
			if (recaptchaResponse == null)
			{
				return false;
			}
			if (recaptchaResponse.IsValid != this.IsValid)
			{
				return false;
			}
			return recaptchaResponse.ErrorCode == this.ErrorCode;
		}

		public override int GetHashCode()
		{
			bool isValid = this.IsValid;
			return isValid.GetHashCode() ^ this.ErrorCode.GetHashCode();
		}
	}
}