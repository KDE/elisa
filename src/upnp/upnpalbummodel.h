/*
 * Copyright 2015-2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef UPNPALBUMMODEL_H
#define UPNPALBUMMODEL_H

#include "upnp/abstractalbummodel.h"

#include "musicalbum.h"
#include "musicaudiotrack.h"

class UpnpAlbumModelPrivate;
class UpnpSsdpEngine;
class UpnpControlAbstractDevice;
class UpnpControlContentDirectory;
class UpnpDiscoveryResult;
class QDomNode;
class MusicStatistics;
class DidlParser;

class UpnpAlbumModel : public AbstractAlbumModel
{
    Q_OBJECT

    Q_PROPERTY(UpnpControlContentDirectory* contentDirectory
               READ contentDirectory
               WRITE setContentDirectory
               NOTIFY contentDirectoryChanged)

    Q_PROPERTY(DidlParser* didlParser
               READ didlParser
               NOTIFY didlParserChanged)

    Q_PROPERTY(QString browseFlag
               READ browseFlag
               WRITE setBrowseFlag
               NOTIFY browseFlagChanged)

    Q_PROPERTY(QString filter
               READ filter
               WRITE setFilter
               NOTIFY filterChanged)

    Q_PROPERTY(QString sortCriteria
               READ sortCriteria
               WRITE setSortCriteria
               NOTIFY sortCriteriaChanged)

    Q_PROPERTY(QString serverName
               READ serverName
               WRITE setServerName
               NOTIFY serverNameChanged)

    Q_PROPERTY(bool useLocalIcons
               READ useLocalIcons
               WRITE setUseLocalIcons
               NOTIFY useLocalIconsChanged)

public:

    explicit UpnpAlbumModel(QObject *parent = 0);

    virtual ~UpnpAlbumModel();

    bool canFetchMore(const QModelIndex &parent) const override;

    void fetchMore(const QModelIndex &parent) override;

    UpnpControlContentDirectory* contentDirectory() const;

    DidlParser* didlParser() const;

    const QString& browseFlag() const;

    const QString& filter() const;

    const QString& sortCriteria() const;

    QString serverName() const;

    bool useLocalIcons() const;

Q_SIGNALS:

    void contentDirectoryChanged();

    void musicDatabaseChanged();

    void didlParserChanged();

    void browseFlagChanged();

    void filterChanged();

    void sortCriteriaChanged();

    void newAlbum(const MusicAlbum &album);

    void newAudioTrack(const MusicAudioTrack &audioTrack);

    void serverNameChanged();

    void useLocalIconsChanged();

public Q_SLOTS:

    void setContentDirectory(UpnpControlContentDirectory *directory);

    void setBrowseFlag(const QString &flag);

    void setFilter(const QString &flag);

    void setSortCriteria(const QString &criteria);

    void setServerName(QString serverName);

    void setUseLocalIcons(bool useLocalIcons);

private Q_SLOTS:

    void contentChanged(const QString &parentId);

private:

    QVariant internalDataAlbum(const QModelIndex &index, int role, DidlParser *currentParser) const;

    QVariant internalDataTrack(const QModelIndex &index, int role, DidlParser *currentParser) const;

    UpnpAlbumModelPrivate *d;

};

#endif // UPNPALBUMMODEL_H
