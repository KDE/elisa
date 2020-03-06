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
    save();

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
    Elisa::ElisaConfiguration::setShowProgressOnTaskBar(mShowProgressInTaskBar);
    Elisa::ElisaConfiguration::setShowSystemTrayIcon(mShowSystemTrayIcon);
    Elisa::ElisaConfiguration::setForceUsageOfFastFileSearch(mForceUsageOfFastFileSearch);

    Elisa::ElisaConfiguration::setEmbeddedView(0);
    switch (mEmbeddedView)
    {
    case ElisaUtils::Unknown:
        Elisa::ElisaConfiguration::setEmbeddedView(0);
        break;
    case ElisaUtils::Album:
        Elisa::ElisaConfiguration::setEmbeddedView(1);
        break;
    case ElisaUtils::Artist:
        Elisa::ElisaConfiguration::setEmbeddedView(2);
        break;
    case ElisaUtils::Genre:
        Elisa::ElisaConfiguration::setEmbeddedView(3);
        break;
    case ElisaUtils::Radio:
    case ElisaUtils::Track:
    case ElisaUtils::Composer:
    case ElisaUtils::FileName:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Container:
        break;
    }

    Elisa::ElisaConfiguration::self()->save();

    mIsDirty = false;
    Q_EMIT isDirtyChanged();
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

void ElisaConfigurationDialog::setForceUsageOfFastFileSearch(bool forceUsageOfFastFileSearch)
{
    if (mForceUsageOfFastFileSearch == forceUsageOfFastFileSearch) {
        return;
    }

    mForceUsageOfFastFileSearch = forceUsageOfFastFileSearch;
    Q_EMIT forceUsageOfFastFileSearchChanged();

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

void ElisaConfigurationDialog::configChanged()
{
    setRootPath(Elisa::ElisaConfiguration::rootPath());
    setShowProgressInTaskBar(Elisa::ElisaConfiguration::showProgressOnTaskBar());
    setShowSystemTrayIcon(Elisa::ElisaConfiguration::showSystemTrayIcon());
    switch (Elisa::ElisaConfiguration::embeddedView())
    {
    case 0:
        setEmbeddedView(ElisaUtils::Unknown);
        break;
    case 1:
        setEmbeddedView(ElisaUtils::Album);
        break;
    case 2:
        setEmbeddedView(ElisaUtils::Artist);
        break;
    case 3:
        setEmbeddedView(ElisaUtils::Genre);
        break;
    }
}

void ElisaConfigurationDialog::setDirty()
{
    mIsDirty = true;
    Q_EMIT isDirtyChanged();
}


#include "moc_elisaconfigurationdialog.cpp"
