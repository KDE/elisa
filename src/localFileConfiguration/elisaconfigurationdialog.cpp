/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisaconfigurationdialog.h"

#include "elisa-version.h"

#include "elisa_settings.h"


#include <QStandardPaths>
#include <QFileInfo>

ElisaConfigurationDialog::ElisaConfigurationDialog(QObject* parent)
    : QObject(parent)
{
    connect(Elisa::ElisaConfiguration::self(), &Elisa::ElisaConfiguration::configChanged,
            this, &ElisaConfigurationDialog::configChanged);
    connect(&mConfigFileWatcher, &QFileSystemWatcher::fileChanged,
            this, &ElisaConfigurationDialog::configChanged);


    setRootPath(Elisa::ElisaConfiguration::rootPath());
    setShowProgressInTaskBar(Elisa::ElisaConfiguration::showProgressOnTaskBar());
    setShowSystemTrayIcon(Elisa::ElisaConfiguration::showSystemTrayIcon());
    setForceUsageOfFastFileSearch(Elisa::ElisaConfiguration::forceUsageOfFastFileSearch());
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

void ElisaConfigurationDialog::configChanged()
{
    setRootPath(Elisa::ElisaConfiguration::rootPath());
}

void ElisaConfigurationDialog::setDirty()
{
    mIsDirty = true;
    Q_EMIT isDirtyChanged();
}


#include "moc_elisaconfigurationdialog.cpp"
