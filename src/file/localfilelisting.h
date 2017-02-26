/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef LOCALFILELISTING_H
#define LOCALFILELISTING_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QHash>
#include <QVector>

#include <memory>

class LocalFileListingPrivate;
class MusicAudioTrack;

class LocalFileListing : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString rootPath
               READ rootPath
               WRITE setRootPath
               NOTIFY rootPathChanged)

public:

    explicit LocalFileListing(QObject *parent = 0);

    virtual ~LocalFileListing();

    QString rootPath() const;

Q_SIGNALS:

    void tracksList(QList<MusicAudioTrack> tracks,
                    const QHash<QString, QUrl> &covers,
                    QString musicSource);

    void removedTracksList(const QList<QUrl> &removedTracks);

    void rootPathChanged();

    void initialTracksListRequired(QString musicSource);

public Q_SLOTS:

    void refreshContent();

    void init();

    void setRootPath(QString rootPath);

    void databaseIsReady();

    void initialTracksList(QString musicSource, QList<MusicAudioTrack> initialList);

private Q_SLOTS:

    void directoryChanged(const QString &path);

    void fileChanged(const QString &modifiedFileName);

private:

    void scanDirectory(const QString &path);

    MusicAudioTrack scanOneFile(QUrl scanFile);

    std::unique_ptr<LocalFileListingPrivate> d;

};



#endif // LOCALFILELISTING_H
