<Query Kind="Program">
  <Connection>
    <ID>fbbd78e1-3a6b-443c-8296-648b2ebb0b61</ID>
    <Persist>true</Persist>
    <Driver>EntityFramework</Driver>
    <Server>CSELLS-XPS\SQLEXPRESS</Server>
    <CustomAssemblyPath>D:\project\mine\sb4\sbda\bin\Release\sbda.dll</CustomAssemblyPath>
    <CustomTypeName>sb4.sellsbrothersEntities</CustomTypeName>
    <CustomMetadataPath>res://sbda/sellsbrothers.csdl|res://sbda/sellsbrothers.ssdl|res://sbda/sellsbrothers.msl</CustomMetadataPath>
    <Database>sellsbrothers</Database>
  </Connection>
</Query>

// from https://help.disqus.com/customer/portal/articles/472150-custom-xml-import-format

void Main() {
	var splits = 1;
	var posts = Posts.OrderBy(c=>c.CreationDate).ToArray();
	var splitTakes = SplitTakes(posts.Length, splits).ToArray();
	
	for(var i = 0; i < splits; i++ ) {
		var file = string.Format(@"c:\temp\sb-comments-{0}.xml", i);
		var splitPosts = posts.Skip(splitTakes[0] * i).Take(splitTakes[i]);
		DumpComments(file, splitPosts);
	}
}

IEnumerable<int> SplitTakes(int total, int splits) {
	var perSplit = total/splits;
	for(var i = 0; i < splits; i++ ) {
		yield return (i == splits - 1 ? perSplit + total%splits : perSplit);
	}
}

void DumpComments(string file, IEnumerable<Post> posts) {
	XNamespace contentNS = "http://purl.org/rss/1.0/modules/content/";
	XNamespace dsqNS = "http://www.disqus.com/";
	XNamespace dcNS = "http://purl.org/dc/elements/1.1/";
	XNamespace wpNS = "http://wordpress.org/export/1.0/";
	
	var channel = new XElement("channel");
	var root = new XElement("rss",
		new XAttribute ("version", "2.0"),
		new XAttribute(XNamespace.Xmlns + "content", contentNS),
		new XAttribute(XNamespace.Xmlns + "dsq", dsqNS),
		new XAttribute(XNamespace.Xmlns + "dc", dcNS),
		new XAttribute(XNamespace.Xmlns + "wp", wpNS),
		channel
	);
	
	foreach(var p in posts) {
		var link = string.Format("http://sellsbrothers.com/posts/details/{0}", p.Id);
		var item = new XElement("item",
			new XElement("title", p.Title),
			new XElement("link", link),
			new XElement(contentNS + "encoded", null),
			new XElement(dsqNS + "thread_identifier", p.Id),
			new XElement(wpNS + "post_date_gmt", p.CreationDate.ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss")),
			new XElement(wpNS + "comment_status", "open")
		);

		foreach( var c in p.Comments.Where(c=>c.IsApproved)) {
			item.Add(new XElement(wpNS + "comment", 
				new XElement(wpNS + "comment_id", c.Id),
				new XElement(wpNS + "comment_author", SafeContent(c.Author, 50)), // Disqus allows 75, but we're trimming extra for HTML encoding expansion
				new XElement(wpNS + "comment_author_email", SafeContent(c.Email, 50)), // ditto
				new XElement(wpNS + "comment_author_url", null),
				new XElement(wpNS + "comment_author_IP", null),
				new XElement(wpNS + "comment_date_gmt", c.CreationDate.ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss")),
				new XElement(wpNS + "comment_content", SafeContent(c.Content, 25000, 3)),
				new XElement(wpNS + "comment_approved", c.IsApproved ? 1 : 0),
				new XElement(wpNS + "comment_parent", 0)
			));
		}
		
		channel.Add(item);
	}
	
	// serialize to XML
	var doc = new XDocument(new XDeclaration("1.0", "utf-8", null));
	doc.Add(root);
	
	var writer = new Utf8StringWriter();
	doc.Save(writer);
	//writer.ToString().Dump();
	File.WriteAllText(file, writer.ToString());
}

// Define other methods and classes here
string SafeContent(string s, int maxLength = int.MaxValue, int minLength = 0) {
	if( s == null ) { s = ""; }
	s = s
			.Replace(((char)0x0).ToString(), "")
			.Replace(((char)0x1).ToString(), "")
			.Replace(((char)0xb).ToString(), "")
			.Replace(((char)0x19).ToString(), "")
			.Replace(((char)0x1b).ToString(), "")
			;
	s = s.Trim();
	if( s.Length < minLength ) { s += new string('-', minLength - s.Length); }
	return s.Substring(0, Math.Min(s.Length, maxLength));
}

class Utf8StringWriter : StringWriter {
   public override Encoding Encoding { get { return Encoding.UTF8; } }
}
