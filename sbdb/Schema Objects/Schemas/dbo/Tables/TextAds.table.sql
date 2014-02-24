-- TODO, merge Badges and TextAds tables and unify in the EDM via inheritance?
CREATE TABLE [dbo].[TextAds](
	[Id] [int] IDENTITY(1,1) NOT NULL,
	[LinkPath] [nvarchar](128) NOT NULL,
	[Title] [nvarchar](128) NOT NULL,
	[ExpirationDate] [datetime] NULL,
	[ContactName] [nvarchar](128) NULL,
	[ContactEmail] [nvarchar](128) NULL,
	[Notes] [nvarchar](max) NULL,
	[Rate] [decimal](18, 0) NULL,
	[RatePeriod] [nvarchar](50) NULL,
	[Canceled] [bit] NOT NULL
) ON [PRIMARY]
