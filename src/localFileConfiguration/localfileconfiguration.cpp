/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "localfileconfiguration.h"

#include "elisa_settings.h"

#include <KPluginFactory>
#include <KAboutData>
#include <KLocalizedString>

#include <QStandardPaths>

K_PLUGIN_FACTORY_WITH_JSON(KCMElisaLocalFileFactory,
                           "kcm_elisa_local_file.json",
                           registerPlugin<KCMElisaLocalFile>();)

KCMElisaLocalFile::KCMElisaLocalFile(QObject* parent, const QVariantList &args)
    : ConfigModule(parent, args)
{
    KAboutData *about = new KAboutData(QStringLiteral("kcm_elisa_local_file"),
                                       i18n("Elisa Local Files Indexer Configuration"),
                                       QStringLiteral("0.1"), {}, KAboutLicense::LGPL_V3,
                                       i18n("Copyright 20017 Matthieu Gallien <matthieu_gallien@yahoo.fr>"));

    about->addAuthor(i18n("Matthieu Gallien"),i18n("Author"), QStringLiteral("mgallien@mgallien.fr"));
    setAboutData(about);

    auto configurationFileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    configurationFileName += QStringLiteral("/elisarc");
    Elisa::ElisaConfiguration::instance(configurationFileName);

    connect(Elisa::ElisaConfiguration::self(), &Elisa::ElisaConfiguration::configChanged,
            this, &KCMElisaLocalFile::configChanged);

    setRootPath(Elisa::ElisaConfiguration::rootPath());
    Elisa::ElisaConfiguration::setRootPath(mRootPath);
    Elisa::ElisaConfiguration::self()->save();
}

KCMElisaLocalFile::~KCMElisaLocalFile()
{
}

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
    if (mRootPath == rootPath) {
        return;
    }

    mRootPath.clear();
    for (const auto &onePath : rootPath) {
        if (onePath.startsWith(QStringLiteral("file:///"))) {
            mRootPath.push_back(onePath.mid(7));
        } else if (onePath.startsWith(QStringLiteral("file:/"))) {
            mRootPath.push_back(onePath.mid(5));
        } else {
            mRootPath.push_back(onePath);
        }
    }

    if (mRootPath.isEmpty()) {
        for (const auto &musicPath : QStandardPaths::standardLocations(QStandardPaths::MusicLocation)) {
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
