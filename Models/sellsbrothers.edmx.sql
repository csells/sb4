
-- --------------------------------------------------
-- Entity Designer DDL Script for SQL Server 2005, 2008, and Azure
-- --------------------------------------------------
-- Date Created: 10/23/2010 17:06:52
-- Generated from EDMX file: D:\project\mine\sb4\Models\sellsbrothers.edmx
-- --------------------------------------------------

SET QUOTED_IDENTIFIER OFF;
GO
USE [sellsbrothers];
GO
IF SCHEMA_ID(N'dbo') IS NULL EXECUTE(N'CREATE SCHEMA [dbo]');
GO

-- --------------------------------------------------
-- Dropping existing FOREIGN KEY constraints
-- --------------------------------------------------

IF OBJECT_ID(N'[dbo].[FK__Badges__SiteId__51300E55]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[Badges] DROP CONSTRAINT [FK__Badges__SiteId__51300E55];
GO
IF OBJECT_ID(N'[dbo].[FK__LandingZo__SiteI__503BEA1C]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[LandingZones] DROP CONSTRAINT [FK__LandingZo__SiteI__503BEA1C];
GO
IF OBJECT_ID(N'[dbo].[FK__OutputCat__SiteI__4F47C5E3]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[OutputCategories] DROP CONSTRAINT [FK__OutputCat__SiteI__4F47C5E3];
GO
IF OBJECT_ID(N'[dbo].[FK__OutputIte__Categ__531856C7]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[OutputItems] DROP CONSTRAINT [FK__OutputIte__Categ__531856C7];
GO
IF OBJECT_ID(N'[dbo].[FK__OutputIte__SiteI__540C7B00]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[OutputItems] DROP CONSTRAINT [FK__OutputIte__SiteI__540C7B00];
GO
IF OBJECT_ID(N'[dbo].[FK__PostCateg__SiteI__5224328E]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[PostCategories] DROP CONSTRAINT [FK__PostCateg__SiteI__5224328E];
GO
IF OBJECT_ID(N'[dbo].[FK__TextAds__SiteId__4E53A1AA]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[Ads] DROP CONSTRAINT [FK__TextAds__SiteId__4E53A1AA];
GO

-- --------------------------------------------------
-- Dropping existing tables
-- --------------------------------------------------

IF OBJECT_ID(N'[dbo].[Ads]', 'U') IS NOT NULL
    DROP TABLE [dbo].[Ads];
GO
IF OBJECT_ID(N'[dbo].[Badges]', 'U') IS NOT NULL
    DROP TABLE [dbo].[Badges];
GO
IF OBJECT_ID(N'[dbo].[fref_link]', 'U') IS NOT NULL
    DROP TABLE [dbo].[fref_link];
GO
IF OBJECT_ID(N'[dbo].[LandingZones]', 'U') IS NOT NULL
    DROP TABLE [dbo].[LandingZones];
GO
IF OBJECT_ID(N'[dbo].[mbPosts]', 'U') IS NOT NULL
    DROP TABLE [dbo].[mbPosts];
GO
IF OBJECT_ID(N'[dbo].[mbTopics]', 'U') IS NOT NULL
    DROP TABLE [dbo].[mbTopics];
GO
IF OBJECT_ID(N'[dbo].[OutputCategories]', 'U') IS NOT NULL
    DROP TABLE [dbo].[OutputCategories];
GO
IF OBJECT_ID(N'[dbo].[OutputItems]', 'U') IS NOT NULL
    DROP TABLE [dbo].[OutputItems];
GO
IF OBJECT_ID(N'[dbo].[PostCategories]', 'U') IS NOT NULL
    DROP TABLE [dbo].[PostCategories];
GO
IF OBJECT_ID(N'[dbo].[Sites]', 'U') IS NOT NULL
    DROP TABLE [dbo].[Sites];
GO

-- --------------------------------------------------
-- Creating all tables
-- --------------------------------------------------

-- Creating table 'Comments'
CREATE TABLE [dbo].[Comments] (
    [Id] int IDENTITY(1,1) NOT NULL,
    [PostId] int  NOT NULL,
    [Author] nvarchar(255)  NULL,
    [Content] nvarchar(max)  NULL,
    [CreationDate] datetime  NOT NULL,
    [IsApproved] bit  NOT NULL,
    [ShouldEmailNotifications] bit  NOT NULL,
    [Email] nvarchar(max)  NULL
);
GO

-- Creating table 'Posts'
CREATE TABLE [dbo].[Posts] (
    [Id] int IDENTITY(1,1) NOT NULL,
    [Title] nvarchar(255)  NULL,
    [Author] nvarchar(255)  NULL,
    [Content] nvarchar(max)  NULL,
    [CreationDate] datetime  NOT NULL,
    [Category] nvarchar(255)  NULL,
    [PostName] nvarchar(max)  NULL,
    [IsActive] bit  NOT NULL
);
GO

-- Creating table 'TinyLinks'
CREATE TABLE [dbo].[TinyLinks] (
    [Code] int  NOT NULL,
    [Link] nvarchar(1024)  NULL,
    [AccessCount] int  NULL,
    [IsActive] bit  NOT NULL,
    [ISBN] nvarchar(16)  NULL
);
GO

-- Creating table 'Badges'
CREATE TABLE [dbo].[Badges] (
    [Id] int IDENTITY(1,1) NOT NULL,
    [SiteId] int  NOT NULL,
    [ImagePath] nvarchar(128)  NULL,
    [LinkPath] nvarchar(128)  NOT NULL,
    [Title] nvarchar(128)  NOT NULL,
    [IsActive] bit  NOT NULL,
    [Order] int  NOT NULL
);
GO

-- Creating table 'LandingZones'
CREATE TABLE [dbo].[LandingZones] (
    [Id] int IDENTITY(1,1) NOT NULL,
    [SiteId] int  NOT NULL,
    [DisplayName] nvarchar(32)  NOT NULL,
    [RelativeUrl] nvarchar(128)  NOT NULL,
    [IsActive] bit  NOT NULL,
    [HeaderImagePath] nvarchar(128)  NOT NULL
);
GO

-- Creating table 'OutputCategories'
CREATE TABLE [dbo].[OutputCategories] (
    [Id] int IDENTITY(1,1) NOT NULL,
    [SiteId] int  NOT NULL,
    [Name] nvarchar(256)  NOT NULL,
    [Order] smallint  NOT NULL
);
GO

-- Creating table 'OutputItems'
CREATE TABLE [dbo].[OutputItems] (
    [Id] int IDENTITY(1,1) NOT NULL,
    [SiteId] int  NOT NULL,
    [CategoryId] int  NOT NULL,
    [Title] nvarchar(1024)  NOT NULL,
    [Link] nvarchar(2048)  NULL,
    [ISBN] nvarchar(16)  NULL,
    [Authors] nvarchar(1024)  NULL,
    [Date] datetime  NULL,
    [Publisher] nvarchar(256)  NULL,
    [Notes] nvarchar(2048)  NULL
);
GO

-- Creating table 'PostCategories'
CREATE TABLE [dbo].[PostCategories] (
    [Id] int IDENTITY(1,1) NOT NULL,
    [SiteId] int  NOT NULL,
    [Name] nvarchar(64)  NULL,
    [DisplayName] nvarchar(128)  NOT NULL
);
GO

-- Creating table 'Sites'
CREATE TABLE [dbo].[Sites] (
    [Id] int IDENTITY(1,1) NOT NULL,
    [BriefAbout] nvarchar(512)  NOT NULL,
    [Title] nvarchar(128)  NOT NULL,
    [ContactEmail] nvarchar(128)  NOT NULL,
    [CopyrightNotice] nvarchar(512)  NOT NULL,
    [PageSize] smallint  NOT NULL,
    [ContactName] nvarchar(128)  NOT NULL
);
GO

-- Creating table 'Ads'
CREATE TABLE [dbo].[Ads] (
    [Id] int IDENTITY(1,1) NOT NULL,
    [LinkPath] nvarchar(128)  NOT NULL,
    [ExpirationDate] datetime  NULL,
    [IsCanceled] bit  NOT NULL,
    [SiteId] int  NOT NULL,
    [Title] nvarchar(128)  NOT NULL,
    [InFooter] bit  NOT NULL
);
GO

-- Creating table 'Images'
CREATE TABLE [dbo].[Images] (
    [Id] int IDENTITY(1,1) NOT NULL
);
GO

-- Creating table 'Ads_TextAd'
CREATE TABLE [dbo].[Ads_TextAd] (
    [Id] int  NOT NULL
);
GO

-- Creating table 'Ads_BannerAd'
CREATE TABLE [dbo].[Ads_BannerAd] (
    [ImagePath] nvarchar(128)  NOT NULL,
    [Id] int  NOT NULL
);
GO

-- --------------------------------------------------
-- Creating all PRIMARY KEY constraints
-- --------------------------------------------------

-- Creating primary key on [Id] in table 'Comments'
ALTER TABLE [dbo].[Comments]
ADD CONSTRAINT [PK_Comments]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'Posts'
ALTER TABLE [dbo].[Posts]
ADD CONSTRAINT [PK_Posts]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Code] in table 'TinyLinks'
ALTER TABLE [dbo].[TinyLinks]
ADD CONSTRAINT [PK_TinyLinks]
    PRIMARY KEY CLUSTERED ([Code] ASC);
GO

-- Creating primary key on [Id] in table 'Badges'
ALTER TABLE [dbo].[Badges]
ADD CONSTRAINT [PK_Badges]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'LandingZones'
ALTER TABLE [dbo].[LandingZones]
ADD CONSTRAINT [PK_LandingZones]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'OutputCategories'
ALTER TABLE [dbo].[OutputCategories]
ADD CONSTRAINT [PK_OutputCategories]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'OutputItems'
ALTER TABLE [dbo].[OutputItems]
ADD CONSTRAINT [PK_OutputItems]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'PostCategories'
ALTER TABLE [dbo].[PostCategories]
ADD CONSTRAINT [PK_PostCategories]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'Sites'
ALTER TABLE [dbo].[Sites]
ADD CONSTRAINT [PK_Sites]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'Ads'
ALTER TABLE [dbo].[Ads]
ADD CONSTRAINT [PK_Ads]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'Images'
ALTER TABLE [dbo].[Images]
ADD CONSTRAINT [PK_Images]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'Ads_TextAd'
ALTER TABLE [dbo].[Ads_TextAd]
ADD CONSTRAINT [PK_Ads_TextAd]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- Creating primary key on [Id] in table 'Ads_BannerAd'
ALTER TABLE [dbo].[Ads_BannerAd]
ADD CONSTRAINT [PK_Ads_BannerAd]
    PRIMARY KEY CLUSTERED ([Id] ASC);
GO

-- --------------------------------------------------
-- Creating all FOREIGN KEY constraints
-- --------------------------------------------------

-- Creating foreign key on [PostId] in table 'Comments'
ALTER TABLE [dbo].[Comments]
ADD CONSTRAINT [FK_ItemComment]
    FOREIGN KEY ([PostId])
    REFERENCES [dbo].[Posts]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;

-- Creating non-clustered index for FOREIGN KEY 'FK_ItemComment'
CREATE INDEX [IX_FK_ItemComment]
ON [dbo].[Comments]
    ([PostId]);
GO

-- Creating foreign key on [SiteId] in table 'Badges'
ALTER TABLE [dbo].[Badges]
ADD CONSTRAINT [FK__Badges__SiteId__51300E55]
    FOREIGN KEY ([SiteId])
    REFERENCES [dbo].[Sites]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;

-- Creating non-clustered index for FOREIGN KEY 'FK__Badges__SiteId__51300E55'
CREATE INDEX [IX_FK__Badges__SiteId__51300E55]
ON [dbo].[Badges]
    ([SiteId]);
GO

-- Creating foreign key on [SiteId] in table 'LandingZones'
ALTER TABLE [dbo].[LandingZones]
ADD CONSTRAINT [FK__LandingZo__SiteI__503BEA1C]
    FOREIGN KEY ([SiteId])
    REFERENCES [dbo].[Sites]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;

-- Creating non-clustered index for FOREIGN KEY 'FK__LandingZo__SiteI__503BEA1C'
CREATE INDEX [IX_FK__LandingZo__SiteI__503BEA1C]
ON [dbo].[LandingZones]
    ([SiteId]);
GO

-- Creating foreign key on [SiteId] in table 'OutputCategories'
ALTER TABLE [dbo].[OutputCategories]
ADD CONSTRAINT [FK__OutputCat__SiteI__4F47C5E3]
    FOREIGN KEY ([SiteId])
    REFERENCES [dbo].[Sites]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;

-- Creating non-clustered index for FOREIGN KEY 'FK__OutputCat__SiteI__4F47C5E3'
CREATE INDEX [IX_FK__OutputCat__SiteI__4F47C5E3]
ON [dbo].[OutputCategories]
    ([SiteId]);
GO

-- Creating foreign key on [CategoryId] in table 'OutputItems'
ALTER TABLE [dbo].[OutputItems]
ADD CONSTRAINT [FK__OutputIte__Categ__531856C7]
    FOREIGN KEY ([CategoryId])
    REFERENCES [dbo].[OutputCategories]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;

-- Creating non-clustered index for FOREIGN KEY 'FK__OutputIte__Categ__531856C7'
CREATE INDEX [IX_FK__OutputIte__Categ__531856C7]
ON [dbo].[OutputItems]
    ([CategoryId]);
GO

-- Creating foreign key on [SiteId] in table 'OutputItems'
ALTER TABLE [dbo].[OutputItems]
ADD CONSTRAINT [FK__OutputIte__SiteI__540C7B00]
    FOREIGN KEY ([SiteId])
    REFERENCES [dbo].[Sites]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;

-- Creating non-clustered index for FOREIGN KEY 'FK__OutputIte__SiteI__540C7B00'
CREATE INDEX [IX_FK__OutputIte__SiteI__540C7B00]
ON [dbo].[OutputItems]
    ([SiteId]);
GO

-- Creating foreign key on [SiteId] in table 'PostCategories'
ALTER TABLE [dbo].[PostCategories]
ADD CONSTRAINT [FK__PostCateg__SiteI__5224328E]
    FOREIGN KEY ([SiteId])
    REFERENCES [dbo].[Sites]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;

-- Creating non-clustered index for FOREIGN KEY 'FK__PostCateg__SiteI__5224328E'
CREATE INDEX [IX_FK__PostCateg__SiteI__5224328E]
ON [dbo].[PostCategories]
    ([SiteId]);
GO

-- Creating foreign key on [SiteId] in table 'Ads'
ALTER TABLE [dbo].[Ads]
ADD CONSTRAINT [FK__TextAds__SiteId__4E53A1AA1]
    FOREIGN KEY ([SiteId])
    REFERENCES [dbo].[Sites]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;

-- Creating non-clustered index for FOREIGN KEY 'FK__TextAds__SiteId__4E53A1AA1'
CREATE INDEX [IX_FK__TextAds__SiteId__4E53A1AA1]
ON [dbo].[Ads]
    ([SiteId]);
GO

-- Creating foreign key on [Id] in table 'Ads_TextAd'
ALTER TABLE [dbo].[Ads_TextAd]
ADD CONSTRAINT [FK_TextAd_inherits_Ad]
    FOREIGN KEY ([Id])
    REFERENCES [dbo].[Ads]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;
GO

-- Creating foreign key on [Id] in table 'Ads_BannerAd'
ALTER TABLE [dbo].[Ads_BannerAd]
ADD CONSTRAINT [FK_BannerAd_inherits_Ad]
    FOREIGN KEY ([Id])
    REFERENCES [dbo].[Ads]
        ([Id])
    ON DELETE NO ACTION ON UPDATE NO ACTION;
GO

-- --------------------------------------------------
-- Script has ended
-- --------------------------------------------------