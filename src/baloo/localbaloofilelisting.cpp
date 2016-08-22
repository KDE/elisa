/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright (C) 2014 Vishesh Handa <me@vhanda.in>
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

#include "localbaloofilelisting.h"

#include "musicaudiotrack.h"

#include <Baloo/Query>
#include <Baloo/File>

#include <KFileMetaData/Properties>

#include <QtDBus/QDBusConnection>

#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <algorithm>

class LocalBalooFileListingPrivate
{
public:

    Baloo::Query mQuery;

    QHash<QString, QVector<MusicAudioTrack>> mAllAlbums;

    QHash<QString, QUrl> mAllAlbumCover;

};

LocalBalooFileListing::LocalBalooFileListing(QObject *parent) : QObject(parent), d(new LocalBalooFileListingPrivate)
{
    d->mQuery.addType(QStringLiteral("Audio"));
}

LocalBalooFileListing::~LocalBalooFileListing()
{
}

void LocalBalooFileListing::init()
{
    QDBusConnection con = QDBusConnection::sessionBus();

    bool connectResult = con.connect(QString(), QStringLiteral("/fileindexer"), QStringLiteral("org.kde.baloo.fileindexer"),
                                QStringLiteral("finishedIndexingFile"), this, SLOT(slotFinishedIndexingFile(QString)));

    if (!connectResult) {
        qDebug() << "problem with baloo monitoring";
    }

    connectResult = con.connect(QString(), QStringLiteral("/files"), QStringLiteral("org.kde"),
                QStringLiteral("changed"), this, SLOT(slotFileMetaDataChanged(QStringList)));

    if (!connectResult) {
        qDebug() << "problem with baloo monitoring";
    }
}

void LocalBalooFileListing::slotFinishedIndexingFile(QString fileName)
{
}

void LocalBalooFileListing::slotFileMetaDataChanged(QStringList fileList)
{
}

void LocalBalooFileListing::refreshContent()
{
    auto resultIterator = d->mQuery.exec();

    int cptTracks = 0;

    while(resultIterator.next()) {
        Baloo::File match(resultIterator.filePath());
        match.load();

        const auto &allProperties = match.properties();

        auto titleProperty = allProperties.find(KFileMetaData::Property::Title);
        auto durationProperty = allProperties.find(KFileMetaData::Property::Duration);
        auto artistProperty = allProperties.find(KFileMetaData::Property::Artist);
        auto albumProperty = allProperties.find(KFileMetaData::Property::Album);
        auto trackNumberProperty = allProperties.find(KFileMetaData::Property::TrackNumber);

        if (albumProperty != allProperties.end()) {
            auto albumValue = albumProperty->toString();
            auto &allTracks = d->mAllAlbums[albumValue];

            MusicAudioTrack newTrack;

            newTrack.setAlbumName(albumValue);
            ++cptTracks;

            if (artistProperty != allProperties.end()) {
                newTrack.setArtist(artistProperty->toString());
            }

            if (durationProperty != allProperties.end()) {
                newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(1000 * durationProperty->toDouble()));
            }

            if (titleProperty != allProperties.end()) {
                newTrack.setTitle(titleProperty->toString());
            }

            if (trackNumberProperty != allProperties.end()) {
                newTrack.setTrackNumber(trackNumberProperty->toInt());
            }

            newTrack.setResourceURI(QUrl::fromLocalFile(resultIterator.filePath()));
            QFileInfo trackFilePath(resultIterator.filePath());
            QFileInfo coverFilePath(trackFilePath.dir().filePath(QStringLiteral("cover.jpg")));
            if (coverFilePath.exists()) {
                d->mAllAlbumCover[albumValue] = QUrl::fromLocalFile(coverFilePath.absoluteFilePath());
            }

            allTracks.push_back(newTrack);
        }

        if (albumProperty != allProperties.end()) {
            auto albumValue = albumProperty->toString();
            auto &allTracks = d->mAllAlbums[albumValue];

            std::sort(allTracks.begin(), allTracks.end());
        }
    }

    Q_EMIT tracksList(d->mAllAlbums, d->mAllAlbumCover);
}


#include "moc_localbaloofilelisting.cpp"
