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

#include "didlparser.h"

#include "upnpcontrolcontentdirectory.h"
#include "upnpcontrolabstractservicereply.h"

#include <QtCore/QVector>
#include <QtCore/QString>

#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>

class DidlParserPrivate
{
public:

    QString mBrowseFlag;

    QString mFilter;

    QString mSortCriteria;

    QString mSearchCriteria;

    QString mParentId;

    UpnpControlContentDirectory *mContentDirectory = nullptr;

    bool mIsDataValid = false;

    QVector<QString> mNewAlbumIds;

    QHash<QString, MusicAlbum> mNewAlbums;

    QVector<QString> mNewMusicTrackIds;

    QHash<QString, MusicAudioTrack> mNewMusicTracks;

};

DidlParser::DidlParser(QObject *parent) : QObject(parent), d(new DidlParserPrivate)
{
}

DidlParser::~DidlParser()
{
}

const QString &DidlParser::browseFlag() const
{
    return d->mBrowseFlag;
}

const QString &DidlParser::filter() const
{
    return d->mFilter;
}

const QString &DidlParser::sortCriteria() const
{
    return d->mSortCriteria;
}

const QString &DidlParser::searchCriteria() const
{
    return d->mSearchCriteria;
}

UpnpControlContentDirectory *DidlParser::contentDirectory() const
{
    return d->mContentDirectory;
}

bool DidlParser::isDataValid() const
{
    return d->mIsDataValid;
}

void DidlParser::setBrowseFlag(const QString &flag)
{
    d->mBrowseFlag = flag;
    Q_EMIT browseFlagChanged();
}

void DidlParser::setFilter(const QString &flag)
{
    d->mFilter = flag;
    Q_EMIT filterChanged();
}

void DidlParser::setSortCriteria(const QString &criteria)
{
    d->mSortCriteria = criteria;
    Q_EMIT sortCriteriaChanged();
}

void DidlParser::setSearchCriteria(const QString &criteria)
{
    d->mSearchCriteria = criteria;
    Q_EMIT searchCriteriaChanged();
}

void DidlParser::setContentDirectory(UpnpControlContentDirectory *directory)
{
    d->mContentDirectory = directory;

    if (!d->mContentDirectory) {
        Q_EMIT contentDirectoryChanged();
        return;
    }

    Q_EMIT contentDirectoryChanged();
}

void DidlParser::setParentId(QString parentId)
{
    if (d->mParentId == parentId)
        return;

    d->mParentId = parentId;
    emit parentIdChanged();
}

void DidlParser::systemUpdateIDChanged()
{
    search();
}

void DidlParser::browse()
{
    auto upnpAnswer = d->mContentDirectory->browse(d->mParentId, d->mBrowseFlag, d->mFilter, 0, 0, d->mSortCriteria);

    connect(upnpAnswer, &UpnpControlAbstractServiceReply::finished, this, &DidlParser::browseFinished);
}

void DidlParser::search()
{
    auto upnpAnswer = d->mContentDirectory->search(d->mParentId, d->mSearchCriteria, d->mFilter, 0, 0, d->mSortCriteria);

    connect(upnpAnswer, &UpnpControlAbstractServiceReply::finished, this, &DidlParser::searchFinished);
}

QString DidlParser::parentId() const
{
    return d->mParentId;
}

const QVector<QString> &DidlParser::newAlbumIds() const
{
    return d->mNewAlbumIds;
}

const QHash<QString, MusicAlbum> &DidlParser::newAlbums() const
{
    return d->mNewAlbums;
}

const QVector<QString> &DidlParser::newMusicTrackIds() const
{
    return d->mNewMusicTrackIds;
}

const QHash<QString, MusicAudioTrack> &DidlParser::newMusicTracks() const
{
    return d->mNewMusicTracks;
}

void DidlParser::browseFinished(UpnpControlAbstractServiceReply *self)
{
    QString result = self->result()[QStringLiteral("Result")].toString();
    bool success = self->success();

    if (!success) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mParentId);

        return;
    }

    QDomDocument browseDescription;
    browseDescription.setContent(result);

    browseDescription.documentElement();

    auto containerList = browseDescription.elementsByTagName(QStringLiteral("container"));
    for (int containerIndex = 0; containerIndex < containerList.length(); ++containerIndex) {
        const QDomNode &containerNode(containerList.at(containerIndex));
        if (!containerNode.isNull()) {
            decodeContainerNode(containerNode, d->mNewAlbums, d->mNewAlbumIds);
        }
    }

    auto itemList = browseDescription.elementsByTagName(QStringLiteral("item"));
    for (int itemIndex = 0; itemIndex < itemList.length(); ++itemIndex) {
        const QDomNode &itemNode(itemList.at(itemIndex));
        if (!itemNode.isNull()) {
            decodeAudioTrackNode(itemNode, d->mNewMusicTracks, d->mNewMusicTrackIds);
        }
    }

    d->mIsDataValid = true;
    Q_EMIT isDataValidChanged(d->mParentId);
}

void DidlParser::searchFinished(UpnpControlAbstractServiceReply *self)
{
    QString result = self->result()[QStringLiteral("Result")].toString();
    bool success = self->success();

    if (!success) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mParentId);

        return;
    }

    QDomDocument browseDescription;
    browseDescription.setContent(result);

    browseDescription.documentElement();

    d->mNewAlbumIds.clear();
    d->mNewAlbums.clear();
    d->mNewMusicTracks.clear();
    d->mNewMusicTrackIds.clear();

    auto containerList = browseDescription.elementsByTagName(QStringLiteral("container"));
    for (int containerIndex = 0; containerIndex < containerList.length(); ++containerIndex) {
        const QDomNode &containerNode(containerList.at(containerIndex));
        if (!containerNode.isNull()) {
            decodeContainerNode(containerNode, d->mNewAlbums, d->mNewAlbumIds);
        }
    }

    auto itemList = browseDescription.elementsByTagName(QStringLiteral("item"));
    for (int itemIndex = 0; itemIndex < itemList.length(); ++itemIndex) {
        const QDomNode &itemNode(itemList.at(itemIndex));
        if (!itemNode.isNull()) {
            decodeAudioTrackNode(itemNode, d->mNewMusicTracks, d->mNewMusicTrackIds);
        }
    }

    d->mIsDataValid = true;
    Q_EMIT isDataValidChanged(d->mParentId);
}

void DidlParser::decodeContainerNode(const QDomNode &containerNode, QHash<QString, MusicAlbum> &newData,
                                     QVector<QString> &newDataIds)
{
    auto parentID = containerNode.toElement().attribute(QStringLiteral("parentID"));
    const auto &id = containerNode.toElement().attribute(QStringLiteral("id"));

    newDataIds.push_back(id);
    auto &chilData = newData[id];

    chilData.mParentId = parentID;
    chilData.mId = id;

    const QString &childCount = containerNode.toElement().attribute(QStringLiteral("childCount"));
    chilData.mTracksCount = childCount.toInt();

    const QDomNode &titleNode = containerNode.firstChildElement(QStringLiteral("dc:title"));
    if (!titleNode.isNull()) {
        chilData.mTitle = titleNode.toElement().text();
    }

    const QDomNode &authorNode = containerNode.firstChildElement(QStringLiteral("upnp:artist"));
    if (!authorNode.isNull()) {
        chilData.mArtist = authorNode.toElement().text();
    }

    const QDomNode &resourceNode = containerNode.firstChildElement(QStringLiteral("res"));
    if (!resourceNode.isNull()) {
        chilData.mResourceURI = QUrl::fromUserInput(resourceNode.toElement().text());
    }

#if 0
    const QDomNode &classNode = containerNode.firstChildElement(QStringLiteral("upnp:class"));
    if (classNode.toElement().text().startsWith(QStringLiteral("object.container.album.musicAlbum"))) {
        chilData[ColumnsRoles::ItemClassRole] = DidlParser::Album;
    } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container.person.musicArtist"))) {
        chilData[ColumnsRoles::ItemClassRole] = DidlParser::Artist;
    } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container"))) {
        chilData[ColumnsRoles::ItemClassRole] = DidlParser::Container;
    }
#endif

    const QDomNode &albumArtNode = containerNode.firstChildElement(QStringLiteral("upnp:albumArtURI"));
    if (!albumArtNode.isNull()) {
        chilData.mAlbumArtURI = QUrl::fromUserInput(albumArtNode.toElement().text());
    }
}

void DidlParser::decodeAudioTrackNode(const QDomNode &itemNode, QHash<QString, MusicAudioTrack> &newData,
                                      QVector<QString> &newDataIds)
{
    const QString &parentID = itemNode.toElement().attribute(QStringLiteral("parentID"));
    const QString &id = itemNode.toElement().attribute(QStringLiteral("id"));

    newDataIds.push_back(id);
    auto &chilData = newData[id];

    chilData.mParentId = parentID;
    chilData.mId = id;

    const QDomNode &titleNode = itemNode.firstChildElement(QStringLiteral("dc:title"));
    if (!titleNode.isNull()) {
        chilData.mTitle = titleNode.toElement().text();
    }

    const QDomNode &authorNode = itemNode.firstChildElement(QStringLiteral("upnp:artist"));
    if (!authorNode.isNull()) {
        chilData.mArtist = authorNode.toElement().text();
    }

    const QDomNode &albumNode = itemNode.firstChildElement(QStringLiteral("upnp:album"));
    if (!albumNode.isNull()) {
        chilData.mAlbumName = albumNode.toElement().text();
    }

    const QDomNode &resourceNode = itemNode.firstChildElement(QStringLiteral("res"));
    if (!resourceNode.isNull()) {
        chilData.mResourceURI = QUrl::fromUserInput(resourceNode.toElement().text());
        if (resourceNode.attributes().contains(QStringLiteral("duration"))) {
            const QDomNode &durationNode = resourceNode.attributes().namedItem(QStringLiteral("duration"));
            QString durationValue = durationNode.nodeValue();
            if (durationValue.startsWith(QStringLiteral("0:"))) {
                durationValue = durationValue.mid(2);
            }
            if (durationValue.contains(uint('.'))) {
                durationValue = durationValue.split(QStringLiteral(".")).first();
            }

            chilData.mDuration = QTime::fromString(durationValue, QStringLiteral("mm:ss"));
            if (!chilData.mDuration.isValid()) {
                chilData.mDuration = QTime::fromString(durationValue, QStringLiteral("hh:mm:ss"));
                if (!chilData.mDuration.isValid()) {
                    chilData.mDuration = QTime::fromString(durationValue, QStringLiteral("hh:mm:ss.z"));
                }
            }
        }

#if 0
        if (resourceNode.attributes().contains(QStringLiteral("artist"))) {
            const QDomNode &artistNode = resourceNode.attributes().namedItem(QStringLiteral("artist"));
            //chilData[ColumnsRoles::ArtistRole] = artistNode.nodeValue();
        }
#endif
    }
}


#include "moc_didlparser.cpp"

