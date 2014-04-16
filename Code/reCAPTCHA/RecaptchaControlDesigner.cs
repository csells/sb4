using System;
using System.ComponentModel.Design;
using System.Web.UI.Design;

namespace Recaptcha.Design
{
	public class RecaptchaControlDesigner : ControlDesigner
	{
		public override DesignerActionListCollection ActionLists
		{
			get
			{
				DesignerActionListCollection designerActionListCollection = new DesignerActionListCollection();
				designerActionListCollection.AddRange(base.ActionLists);
				designerActionListCollection.Add(new RecaptchaControlDesigner.ActionList(this));
				return designerActionListCollection;
			}
		}

		public override bool AllowResize
		{
			get
			{
				return false;
			}
		}

		public RecaptchaControlDesigner()
		{
		}

		public override string GetDesignTimeHtml()
		{
			return base.CreatePlaceHolderDesignTimeHtml("reCAPTCHA Validator");
		}

		public class ActionList : DesignerActionList
		{
			private RecaptchaControlDesigner _parent;

			public ActionList(RecaptchaControlDesigner parent) : base(parent.Component)
			{
				this._parent = parent;
			}

			public override DesignerActionItemCollection GetSortedActionItems()
			{
				DesignerActionItemCollection designerActionItemCollection = new DesignerActionItemCollection();
				designerActionItemCollection.Add(new DesignerActionHeaderItem("API Key"));
				designerActionItemCollection.Add(new DesignerActionTextItem("To use reCAPTCHA, you need an API key from http://admin.recaptcha.net/", string.Empty));
				return designerActionItemCollection;
			}
		}
	}
}