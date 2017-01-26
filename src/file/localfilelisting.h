/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QHash>
#include <QtCore/QVector>

#include <memory>

class LocalFileListingPrivate;
class MusicAudioTrack;

class LocalFileListing : public QObject
{

    Q_OBJECT

public:

    explicit LocalFileListing(QObject *parent = 0);

    virtual ~LocalFileListing();

Q_SIGNALS:

    void tracksList(const QHash<QString, QVector<MusicAudioTrack>> &tracks,
                    const QHash<QString, QUrl> &covers);

    void removedTracksList(const QList<QString> &removedTracks);

public Q_SLOTS:

    void refreshContent();

    void init();

private Q_SLOTS:

    void directoryChanged(const QString &path);

    void fileChanged(const QString &path);

private:

    void scanDirectory(const QString &path);

    std::unique_ptr<LocalFileListingPrivate> d;

};



#endif // LOCALFILELISTING_H
