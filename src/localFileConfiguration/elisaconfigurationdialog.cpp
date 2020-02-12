/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
