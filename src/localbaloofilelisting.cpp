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

    QHash<QString, QString> mAllAlbumCover;

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

    connectResult = con.connect(QString(), QStringLiteral("/files"), QStringLiteral("org.kde"),
                QStringLiteral("changed"), this, SLOT(slotFileMetaDataChanged(QStringList)));
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

    qDebug() << "LocalBalooFileListing::refreshContent";

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

            newTrack.mAlbumName = albumValue;
            ++cptTracks;

            if (artistProperty != allProperties.end()) {
                newTrack.mArtist = artistProperty->toString();
            }

            if (durationProperty != allProperties.end()) {
                newTrack.mDuration = QTime::fromMSecsSinceStartOfDay(1000 * durationProperty->toDouble());
            }

            if (titleProperty != allProperties.end()) {
                newTrack.mTitle = titleProperty->toString();
            }

            if (trackNumberProperty != allProperties.end()) {
                newTrack.mTrackNumber = trackNumberProperty->toInt();
            }

            newTrack.mResourceURI = QUrl::fromLocalFile(resultIterator.filePath());
            QFileInfo trackFilePath(resultIterator.filePath());
            QFileInfo coverFilePath(trackFilePath.dir().filePath(QStringLiteral("cover.jpg")));
            if (coverFilePath.exists()) {
                d->mAllAlbumCover[albumValue] = coverFilePath.absoluteFilePath();
            }

            allTracks.push_back(newTrack);
        }

        if (albumProperty != allProperties.end()) {
            auto albumValue = albumProperty->toString();
            auto &allTracks = d->mAllAlbums[albumValue];

            std::sort(allTracks.begin(), allTracks.end(), [](const auto &left, const auto &right) {return left.mTrackNumber <= right.mTrackNumber;});
        }
    }

    qDebug() << d->mAllAlbums.size();
    qDebug() << d->mAllAlbums.keys();
    qDebug() << cptTracks;

    Q_EMIT tracksList(d->mAllAlbums, d->mAllAlbumCover);
}


#include "moc_localbaloofilelisting.cpp"
