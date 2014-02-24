create table Sites (
    Id int identity (1, 1) not null,
    BriefAbout nvarchar (512) not null,
    Title nvarchar (128) not null,
    Contact nvarchar (128) not null,
	CopyrightNotice nvarchar(512) not null,
	TopPosts smallint not null default 15,
);

