/*
 * Copyright 2015-2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef DIDLPARSER_H
#define DIDLPARSER_H

#include "musicaudiotrack.h"

#include <QObject>
#include <QList>
#include <QHash>
#include <QVariant>
#include <QString>

#include <memory>

class UpnpControlAbstractServiceReply;
class QDomNode;
class UpnpControlContentDirectory;
class DidlParserPrivate;

class DidlParser : public QObject
{

    Q_OBJECT

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

    Q_PROPERTY(QString searchCriteria
               READ searchCriteria
               WRITE setSearchCriteria
               NOTIFY searchCriteriaChanged)

    Q_PROPERTY(QString parentId
               READ parentId
               WRITE setParentId
               NOTIFY parentIdChanged)

    Q_PROPERTY(UpnpControlContentDirectory* contentDirectory
               READ contentDirectory
               WRITE setContentDirectory
               NOTIFY contentDirectoryChanged)

    Q_PROPERTY(bool isDataValid
               READ isDataValid
               NOTIFY isDataValidChanged)

public:

    explicit DidlParser(QObject *parent = nullptr);

    ~DidlParser() override;

    const QString& browseFlag() const;

    const QString& filter() const;

    const QString& sortCriteria() const;

    const QString& searchCriteria() const;

    UpnpControlContentDirectory* contentDirectory() const;

    bool isDataValid() const;

    void browse(int startIndex = 0, int maximumNmberOfResults = 0);

    void search(int startIndex = 0, int maximumNumberOfResults = 0);

    QString parentId() const;

    const QVector<QString> &newAlbumIds() const;

    const QVector<QString> &newMusicTrackIds() const;

    const QList<MusicAudioTrack> &newMusicTracks() const;

    const QHash<QString, QUrl>& covers() const;

Q_SIGNALS:

    void browseFlagChanged();

    void filterChanged();

    void sortCriteriaChanged();

    void searchCriteriaChanged();

    void contentDirectoryChanged();

    void isDataValidChanged(const QString &uuid, const QString &parentId);

    void parentIdChanged();

public Q_SLOTS:

    void setBrowseFlag(const QString &flag);

    void setFilter(const QString &flag);

    void setSortCriteria(const QString &criteria);

    void setSearchCriteria(const QString &criteria);

    void setContentDirectory(UpnpControlContentDirectory *directory);

    void setParentId(QString parentId);

    void systemUpdateIDChanged();

private Q_SLOTS:

    void browseFinished(UpnpControlAbstractServiceReply *self);

    void searchFinished(UpnpControlAbstractServiceReply *self);

private:

    void decodeContainerNode(const QDomNode &containerNode, QHash<QString, MusicAudioTrack> &newData, QVector<QString> &newDataIds);

    void decodeAudioTrackNode(const QDomNode &itemNode, QHash<QString, MusicAudioTrack> &newData, QVector<QString> &newDataIds);

    void groupNewTracksByAlbums();

    std::unique_ptr<DidlParserPrivate> d;

};

#endif // DIDLPARSER_H
