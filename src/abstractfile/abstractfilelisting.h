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

#ifndef ABSTRACTFILELISTING_H
#define ABSTRACTFILELISTING_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QHash>
#include <QVector>

#include <memory>

class AbstractFileListingPrivate;
class MusicAudioTrack;

class AbstractFileListing : public QObject
{

    Q_OBJECT

public:

    explicit AbstractFileListing(const QString &sourceName, QObject *parent = 0);

    virtual ~AbstractFileListing();

Q_SIGNALS:

    void tracksList(QList<MusicAudioTrack> tracks, const QHash<QString, QUrl> &covers, QString musicSource);

    void removedTracksList(const QList<QUrl> &removedTracks);

    void modifyTracksList(const QList<MusicAudioTrack> &modifiedTracks, const QHash<QString, QUrl> &covers);

    void initialTracksListRequired(QString musicSource);

public Q_SLOTS:

    void refreshContent();

    void init();

    void databaseIsReady();

    void newTrackFile(MusicAudioTrack partialTrack);

protected Q_SLOTS:

    void directoryChanged(const QString &path);

    void fileChanged(const QString &modifiedFileName);

protected:

    virtual void triggerRefreshOfContent();

    void scanDirectory(const QUrl &path);

    const QString &sourceName() const;

    virtual MusicAudioTrack scanOneFile(QUrl scanFile);

    void watchPath(const QString &pathName);

private:

    std::unique_ptr<AbstractFileListingPrivate> d;

};



#endif // ABSTRACTFILELISTING_H
