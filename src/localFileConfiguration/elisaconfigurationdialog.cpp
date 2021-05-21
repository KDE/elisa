/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisaconfigurationdialog.h"

#include "elisa-version.h"

#include "elisa_settings.h"


#include <QStandardPaths>
#include <QFileInfo>
#include <QTimer>

ElisaConfigurationDialog::ElisaConfigurationDialog(QObject* parent)
    : QObject(parent)
{
    connect(Elisa::ElisaConfiguration::self(), &Elisa::ElisaConfiguration::configChanged,
            this, &ElisaConfigurationDialog::configChanged);
    connect(&mConfigFileWatcher, &QFileSystemWatcher::fileChanged,
            this, &ElisaConfigurationDialog::configChanged);

    configChanged();

    mConfigFileWatcher.addPath(Elisa::ElisaConfiguration::self()->config()->name());
}

ElisaConfigurationDialog::~ElisaConfigurationDialog()
= default;

QStringList ElisaConfigurationDialog::rootPath() const
{
    return mRootPath;
}

void ElisaConfigurationDialog::setRootPath(const QStringList &rootPath)
{
    if (mRootPath == rootPath && !mRootPath.isEmpty()) {
        return;
    }

    mRootPath.clear();
    for (const auto &onePath : rootPath) {
        auto workPath = onePath;
        if (workPath.startsWith(QLatin1String("file:/"))) {
            auto urlPath = QUrl{workPath};
            workPath = urlPath.toLocalFile();
        }

        QFileInfo pathFileInfo(workPath);
        auto directoryPath = pathFileInfo.canonicalFilePath();
        if (!directoryPath.isEmpty()) {
            mRootPath.push_back(directoryPath);
        }
    }

    if (mRootPath.isEmpty()) {
        auto systemMusicPaths = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
        for (const auto &musicPath : qAsConst(systemMusicPaths)) {
            mRootPath.push_back(musicPath);
        }
    }

    Q_EMIT rootPathChanged(mRootPath);

    setDirty();
}

void ElisaConfigurationDialog::save()
{
    Elisa::ElisaConfiguration::setRootPath(mRootPath);
    Elisa::ElisaConfiguration::setShowNowPlayingBackground(mShowNowPlayingBackground);
    Elisa::ElisaConfiguration::setShowProgressOnTaskBar(mShowProgressInTaskBar);
    Elisa::ElisaConfiguration::setShowSystemTrayIcon(mShowSystemTrayIcon);
    Elisa::ElisaConfiguration::setForceUsageOfFastFileSearch(!mForceUsageOfSlowFileSystemIndexing);
    Elisa::ElisaConfiguration::setPlayAtStartup(mPlayAtStartup);
    Elisa::ElisaConfiguration::setColorScheme(mColorScheme);

    Elisa::ElisaConfiguration::setEmbeddedView(Elisa::ElisaConfiguration::EnumEmbeddedView::NoView);
    switch (mEmbeddedView)
    {
    case ElisaUtils::Unknown:
        Elisa::ElisaConfiguration::setEmbeddedView(Elisa::ElisaConfiguration::EnumEmbeddedView::NoView);
        break;
    case ElisaUtils::Album:
        Elisa::ElisaConfiguration::setEmbeddedView(Elisa::ElisaConfiguration::EnumEmbeddedView::AllAlbums);
        break;
    case ElisaUtils::Artist:
        Elisa::ElisaConfiguration::setEmbeddedView(Elisa::ElisaConfiguration::EnumEmbeddedView::AllArtists);
        break;
    case ElisaUtils::Genre:
        Elisa::ElisaConfiguration::setEmbeddedView(Elisa::ElisaConfiguration::EnumEmbeddedView::AllGenres);
        break;
    case ElisaUtils::Radio:
    case ElisaUtils::Track:
    case ElisaUtils::Composer:
    case ElisaUtils::FileName:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Container:
        break;
    }

    Elisa::ElisaConfiguration::setInitialView(mInitialViewIndex);

    Elisa::ElisaConfiguration::self()->save();

    mIsDirty = false;
    Q_EMIT isDirtyChanged();
}

void ElisaConfigurationDialog::cancel()
{
    if (mIsDirty) {
        configChanged();

        mIsDirty = false;
        Q_EMIT isDirtyChanged();
    }
}

void ElisaConfigurationDialog::setShowNowPlayingBackground(bool showNowPlayingBackground)
{
    if (mShowNowPlayingBackground == showNowPlayingBackground) {
        return;
    }

    mShowNowPlayingBackground = showNowPlayingBackground;
    Q_EMIT showNowPlayingBackgroundChanged();

    setDirty();
}

void ElisaConfigurationDialog::setShowProgressInTaskBar(bool showProgressInTaskBar)
{
    if (mShowProgressInTaskBar == showProgressInTaskBar) {
        return;
    }

    mShowProgressInTaskBar = showProgressInTaskBar;
    Q_EMIT showProgressInTaskBarChanged();

    setDirty();
}

void ElisaConfigurationDialog::setShowSystemTrayIcon(bool showSystemTrayIcon)
{
    if (mShowSystemTrayIcon == showSystemTrayIcon) {
        return;
    }

    mShowSystemTrayIcon = showSystemTrayIcon;
    Q_EMIT showSystemTrayIconChanged();

    setDirty();
}

void ElisaConfigurationDialog::setForceUsageOfSlowFileSystemIndexing(bool forceUsageOfSlowFileSystemIndexing)
{
    if (mForceUsageOfSlowFileSystemIndexing == forceUsageOfSlowFileSystemIndexing) {
        return;
    }

    mForceUsageOfSlowFileSystemIndexing = forceUsageOfSlowFileSystemIndexing;
    Q_EMIT forceUsageOfSlowFileSystemIndexingChanged();

    setDirty();
}

void ElisaConfigurationDialog::setEmbeddedView(ElisaUtils::PlayListEntryType embeddedView)
{
    if (mEmbeddedView == embeddedView) {
        return;
    }

    mEmbeddedView = embeddedView;
    QTimer::singleShot(0, [this](){ Q_EMIT embeddedViewChanged(); });

    setDirty();
}

void ElisaConfigurationDialog::setInitialViewIndex(int initialViewIndex)
{
    if (mInitialViewIndex == initialViewIndex) {
        return;
    }

    mInitialViewIndex = initialViewIndex;
    QTimer::singleShot(0, [this](){ Q_EMIT initialViewIndexChanged(); });

    setDirty();
}

void ElisaConfigurationDialog::setPlayAtStartup(bool playAtStartup)
{
    if (mPlayAtStartup == playAtStartup) {
        return;
    }
    mPlayAtStartup = playAtStartup;
    Q_EMIT playAtStartupChanged();

    setDirty();
}

void ElisaConfigurationDialog::setColorScheme(const QString &scheme)
{
    if (mColorScheme == scheme) {
        return;
    }
    mColorScheme = scheme;
    Q_EMIT colorSchemeChanged();

    setDirty();
}

void ElisaConfigurationDialog::removeMusicLocation(QString location)
{
    mRootPath.removeAll(location);
    Q_EMIT rootPathChanged(mRootPath);
}

void ElisaConfigurationDialog::configChanged()
{
    mRootPath = Elisa::ElisaConfiguration::rootPath();
    Q_EMIT rootPathChanged(mRootPath);

    mShowNowPlayingBackground = Elisa::ElisaConfiguration::showNowPlayingBackground();
    Q_EMIT showNowPlayingBackgroundChanged();

    mShowProgressInTaskBar = Elisa::ElisaConfiguration::showProgressOnTaskBar();
    Q_EMIT showProgressInTaskBarChanged();

    mShowSystemTrayIcon = Elisa::ElisaConfiguration::showSystemTrayIcon();
    Q_EMIT showSystemTrayIconChanged();

    mPlayAtStartup = Elisa::ElisaConfiguration::playAtStartup();
    Q_EMIT playAtStartupChanged();

    mColorScheme = Elisa::ElisaConfiguration::colorScheme();
    Q_EMIT colorSchemeChanged();

    mForceUsageOfSlowFileSystemIndexing = !Elisa::ElisaConfiguration::forceUsageOfFastFileSearch();
    Q_EMIT forceUsageOfSlowFileSystemIndexingChanged();

    switch (Elisa::ElisaConfiguration::embeddedView())
    {
    case Elisa::ElisaConfiguration::EnumEmbeddedView::NoView:
        mEmbeddedView = ElisaUtils::Unknown;
        break;
    case Elisa::ElisaConfiguration::EnumEmbeddedView::AllAlbums:
        mEmbeddedView = ElisaUtils::Album;
        break;
    case Elisa::ElisaConfiguration::EnumEmbeddedView::AllArtists:
        mEmbeddedView = ElisaUtils::Artist;
        break;
    case Elisa::ElisaConfiguration::EnumEmbeddedView::AllGenres:
        mEmbeddedView = ElisaUtils::Genre;
        break;
    }
    Q_EMIT embeddedViewChanged();

    mInitialViewIndex = Elisa::ElisaConfiguration::initialView();
    Q_EMIT initialViewIndexChanged();
}

void ElisaConfigurationDialog::setDirty()
{
    mIsDirty = true;
    Q_EMIT isDirtyChanged();
}


#include "moc_elisaconfigurationdialog.cpp"
