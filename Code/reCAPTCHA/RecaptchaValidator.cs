using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Web;

namespace Recaptcha
{
	public class RecaptchaValidator
	{
		private const string VerifyUrl = "http://api-verify.recaptcha.net/verify";

		private string privateKey;

		private string remoteIp;

		private string challenge;

		private string response;

		public string Challenge
		{
			get
			{
				return this.challenge;
			}
			set
			{
				this.challenge = value;
			}
		}

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

		public string RemoteIP
		{
			get
			{
				return this.remoteIp;
			}
			set
			{
				IPAddress pAddress = IPAddress.Parse(value);
				if (pAddress == null || pAddress.AddressFamily != AddressFamily.InterNetwork && pAddress.AddressFamily != AddressFamily.InterNetworkV6)
				{
					throw new ArgumentException(string.Concat("Expecting an IP address, got ", pAddress));
				}
				this.remoteIp = pAddress.ToString();
			}
		}

		public string Response
		{
			get
			{
				return this.response;
			}
			set
			{
				this.response = value;
			}
		}

		public RecaptchaValidator()
		{
		}

		private void CheckNotNull(object obj, string name)
		{
			if (obj == null)
			{
				throw new ArgumentNullException(name);
			}
		}

		public RecaptchaResponse Validate()
		{
			string[] strArrays;
			RecaptchaResponse recaptchaNotReachable;
			this.CheckNotNull(this.PrivateKey, "PrivateKey");
			this.CheckNotNull(this.RemoteIP, "RemoteIp");
			this.CheckNotNull(this.Challenge, "Challenge");
			this.CheckNotNull(this.Response, "Response");
			if (this.challenge == string.Empty || this.response == string.Empty)
			{
				return RecaptchaResponse.InvalidSolution;
			}
			HttpWebRequest version10 = (HttpWebRequest)WebRequest.Create("http://api-verify.recaptcha.net/verify");
			version10.ProtocolVersion = HttpVersion.Version10;
			version10.Timeout = 30000;
			version10.Method = "POST";
			version10.UserAgent = "reCAPTCHA/ASP.NET";
			version10.ContentType = "application/x-www-form-urlencoded";
			object[] objArray = new object[] { HttpUtility.UrlEncode(this.PrivateKey), HttpUtility.UrlEncode(this.RemoteIP), HttpUtility.UrlEncode(this.Challenge), HttpUtility.UrlEncode(this.Response) };
			string str = string.Format("privatekey={0}&remoteip={1}&challenge={2}&response={3}", objArray);
			byte[] bytes = Encoding.ASCII.GetBytes(str);
			using (Stream requestStream = version10.GetRequestStream())
			{
				requestStream.Write(bytes, 0, (int)bytes.Length);
			}
			try
			{
				using (WebResponse response = version10.GetResponse())
				{
					using (TextReader streamReader = new StreamReader(response.GetResponseStream(), Encoding.UTF8))
					{
						strArrays = streamReader.ReadToEnd().Split(new char[0]);
					}
				}
				string str1 = strArrays[0];
				string str2 = str1;
				if (str1 != null)
				{
					if (str2 == "true")
					{
						return RecaptchaResponse.Valid;
					}
					if (str2 == "false")
					{
						return new RecaptchaResponse(false, strArrays[1]);
					}
				}
				throw new InvalidProgramException("Unknown status response.");
			}
			catch (WebException webException)
			{
				EventLog.WriteEntry("Application", webException.Message, EventLogEntryType.Error);
				recaptchaNotReachable = RecaptchaResponse.RecaptchaNotReachable;
			}
			return recaptchaNotReachable;
		}
	}
}