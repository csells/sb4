CREATE TABLE [dbo].[Badges] (
    [Id] INT IDENTITY (1, 1) NOT NULL,
    [ImagePath] NVARCHAR (128) NULL,
    [LinkPath] NVARCHAR (128) NOT NULL,
    [Title] NVARCHAR (128) NOT NULL,
	[IsActive] bit not null default 1,
	[Order] int not null default 2147483647, -- TODO, SQL equivalent of Int.MaxValue
);