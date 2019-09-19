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

#include "localfileconfiguration.h"

#include "elisa-version.h"

#include "elisa_settings.h"

#include <KPluginFactory>
#include <KAboutData>
#include <KLocalizedString>

#include <QStandardPaths>

K_PLUGIN_CLASS_WITH_JSON(KCMElisaLocalFile, "kcm_elisa_local_file.json")

KCMElisaLocalFile::KCMElisaLocalFile(QObject* parent, const QVariantList &args)
    : ConfigModule(parent, args)
{
    KAboutData *about = new KAboutData(QStringLiteral("kcm_elisa_local_file"),
                                       i18n("Elisa Local Files Indexer Configuration"),
                                       QStringLiteral(ELISA_VERSION_STRING), {}, KAboutLicense::LGPL_V3,
                                       i18n("Copyright 2017-2019 Matthieu Gallien <matthieu_gallien@yahoo.fr>"));

    about->addAuthor(i18n("Matthieu Gallien"),i18n("Author"), QStringLiteral("mgallien@mgallien.fr"));
    setAboutData(about);

    auto configurationFileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    configurationFileName += QStringLiteral("/elisarc");
    Elisa::ElisaConfiguration::instance(configurationFileName);

    connect(Elisa::ElisaConfiguration::self(), &Elisa::ElisaConfiguration::configChanged,
            this, &KCMElisaLocalFile::configChanged);
    connect(&mConfigFileWatcher, &QFileSystemWatcher::fileChanged,
            this, &KCMElisaLocalFile::configChanged);


    setRootPath(Elisa::ElisaConfiguration::rootPath());
    Elisa::ElisaConfiguration::setRootPath(mRootPath);
    Elisa::ElisaConfiguration::self()->save();

    mConfigFileWatcher.addPath(Elisa::ElisaConfiguration::self()->config()->name());
}

KCMElisaLocalFile::~KCMElisaLocalFile()
= default;

QStringList KCMElisaLocalFile::rootPath() const
{
    return mRootPath;
}

void KCMElisaLocalFile::defaults()
{
    setRootPath(QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
}

void KCMElisaLocalFile::load()
{
    setRootPath(Elisa::ElisaConfiguration::rootPath());
}

void KCMElisaLocalFile::save()
{
    Elisa::ElisaConfiguration::setRootPath(mRootPath);
    Elisa::ElisaConfiguration::self()->save();
}

void KCMElisaLocalFile::setRootPath(QStringList rootPath)
{
    if (mRootPath == rootPath && !mRootPath.isEmpty()) {
        return;
    }

    mRootPath.clear();
    for (const auto &onePath : rootPath) {
        if (onePath.startsWith(QLatin1String("file:///"))) {
            mRootPath.push_back(onePath.mid(7));
        } else if (onePath.startsWith(QLatin1String("file:/"))) {
            mRootPath.push_back(onePath.mid(5));
        } else {
            mRootPath.push_back(onePath);
        }
    }

    if (mRootPath.isEmpty()) {
        auto systemMusicPaths = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
        for (const auto &musicPath : qAsConst(systemMusicPaths)) {
            mRootPath.push_back(musicPath);
        }
    }

    Q_EMIT rootPathChanged(mRootPath);

    setNeedsSave(true);
    Q_EMIT needsSaveChanged();
}

void KCMElisaLocalFile::configChanged()
{
    setRootPath(Elisa::ElisaConfiguration::rootPath());
}


#include "localfileconfiguration.moc"
