create table LandingZones
(
    Id int identity (1, 1) not null,
    DisplayName nvarchar (32) not null,
	[Path] nvarchar (128) not null,
	IsTopLevel bit not null,
	HeaderImagePath nvarchar (128) not null,
	Category nvarchar (128) null,
)
