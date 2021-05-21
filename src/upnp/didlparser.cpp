/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "didlparser.h"

#include "upnpcontrolcontentdirectory.h"
#include "upnpcontrolabstractservicereply.h"
#include "upnpservicedescription.h"
#include "upnpdevicedescription.h"
#include "elisautils.h"

#include "upnpLogging.h"

#include <QVector>
#include <QString>

#include <QDomDocument>
#include <QDomNode>

class DidlParserPrivate
{
public:

    QString mBrowseFlag = QStringLiteral("*");

    QString mFilter = QStringLiteral("*");

    QString mSortCriteria;

    QString mSearchCriteria;

    QString mParentId;

    QString mDeviceUUID;

    UpnpControlContentDirectory *mContentDirectory = nullptr;

    QVector<QString> mNewMusicTrackIds;

    QHash<QString, DataTypes::UpnpTrackDataType> mNewMusicTracks;

    QHash<QString, QVector<DataTypes::UpnpTrackDataType>> mNewTracksByAlbums;

    QHash<QString, QUrl> mCovers;

    bool mIsDataValid = false;

};

DidlParser::DidlParser(QObject *parent) : QObject(parent), d(new DidlParserPrivate)
{
}

DidlParser::~DidlParser()
= default;

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

void DidlParser::setBrowseFlag(QString flag)
{
    if (d->mBrowseFlag == flag) {
        return;
    }

    d->mBrowseFlag = std::move(flag);
    Q_EMIT browseFlagChanged();
}

void DidlParser::setFilter(QString flag)
{
    if (d->mFilter == flag) {
        return;
    }

    d->mFilter = std::move(flag);
    Q_EMIT filterChanged();
}

void DidlParser::setSortCriteria(QString criteria)
{
    if (d->mSortCriteria == criteria) {
        return;
    }

    d->mSortCriteria = std::move(criteria);
    Q_EMIT sortCriteriaChanged();
}

void DidlParser::setSearchCriteria(QString criteria)
{
    if (d->mSearchCriteria == criteria) {
        return;
    }

    d->mSearchCriteria = std::move(criteria);
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
    if (d->mParentId == parentId) {
        return;
    }

    d->mParentId = std::move(parentId);
    Q_EMIT parentIdChanged();
}

void DidlParser::setDeviceUUID(QString deviceUUID)
{
    if (d->mDeviceUUID == deviceUUID) {
        return;
    }

    d->mDeviceUUID = std::move(deviceUUID);
    Q_EMIT deviceUUIDChanged();
}

void DidlParser::systemUpdateIDChanged()
{
    search();
}

void DidlParser::browse(int startIndex, int maximumNmberOfResults)
{
    qCDebug(orgKdeElisaUpnp()) << "DidlParser::browse" << d->mParentId << d->mBrowseFlag << d->mFilter << startIndex << maximumNmberOfResults << d->mSortCriteria;

    auto upnpAnswer = d->mContentDirectory->browse(d->mParentId, d->mBrowseFlag, d->mFilter, startIndex, maximumNmberOfResults, d->mSortCriteria);

    if (startIndex == 0) {
        d->mNewMusicTracks.clear();
        d->mNewMusicTrackIds.clear();
        d->mCovers.clear();
    }

    connect(upnpAnswer, &UpnpControlAbstractServiceReply::finished, this, &DidlParser::browseFinished);
}

void DidlParser::search(int startIndex, int maximumNumberOfResults)
{
    if (!d->mContentDirectory) {
        return;
    }

    if (startIndex == 0) {
        d->mNewMusicTracks.clear();
        d->mNewMusicTrackIds.clear();
        d->mCovers.clear();
    }

    auto upnpAnswer = d->mContentDirectory->search(d->mParentId, d->mSearchCriteria, d->mFilter, startIndex, maximumNumberOfResults, d->mSortCriteria);

    connect(upnpAnswer, &UpnpControlAbstractServiceReply::finished, this, &DidlParser::searchFinished);
}

QString DidlParser::parentId() const
{
    return d->mParentId;
}

const QString &DidlParser::deviceUUID() const
{
    return d->mDeviceUUID;
}

const QVector<QString> &DidlParser::newMusicTrackIds() const
{
    return d->mNewMusicTrackIds;
}

const QHash<QString, DataTypes::UpnpTrackDataType> &DidlParser::newMusicTracks() const
{
    return d->mNewMusicTracks;
}

const QHash<QString, QUrl> &DidlParser::covers() const
{
    return d->mCovers;
}

void DidlParser::browseFinished(UpnpControlAbstractServiceReply *self)
{
    qCDebug(orgKdeElisaUpnp()) << "DidlParser::browseFinished";

    const auto &resultData = self->result();

    bool success = self->success();

    if (!success) {
        qCDebug(orgKdeElisaUpnp()) << "DidlParser::browseFinished" << "error" << self->error();

        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mParentId);

        return;
    }

    QString result = resultData[QStringLiteral("Result")].toString();

    bool intConvert;
    auto numberReturned = resultData[QStringLiteral("NumberReturned")].toInt(&intConvert);

    if (!intConvert) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mParentId);

        return;
    }

    auto totalMatches = resultData[QStringLiteral("TotalMatches")].toInt(&intConvert);

    if (!intConvert) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mParentId);

        return;
    }

    if (totalMatches > numberReturned) {
        browse(d->mNewMusicTracks.size() + numberReturned);
    }

    QDomDocument browseDescription;
    browseDescription.setContent(result);

    browseDescription.documentElement();

    auto containerList = browseDescription.elementsByTagName(QStringLiteral("container"));
    for (int containerIndex = 0; containerIndex < containerList.length(); ++containerIndex) {
        const QDomNode &containerNode(containerList.at(containerIndex));
        if (!containerNode.isNull()) {
            decodeContainerNode(containerNode, d->mNewMusicTracks, d->mNewMusicTrackIds);
        }
    }

    auto itemList = browseDescription.elementsByTagName(QStringLiteral("item"));
    for (int itemIndex = 0; itemIndex < itemList.length(); ++itemIndex) {
        const QDomNode &itemNode(itemList.at(itemIndex));
        if (!itemNode.isNull()) {
            qCInfo(orgKdeElisaUpnp()) << "DidlParser::browseFinished" << "new track node" << itemNode.toDocument().toString();

            decodeAudioTrackNode(itemNode, d->mNewMusicTracks, d->mNewMusicTrackIds);
        }
    }

    groupNewTracksByAlbums();
    d->mIsDataValid = true;
    Q_EMIT isDataValidChanged(d->mParentId);
}

void DidlParser::groupNewTracksByAlbums()
{
    d->mNewTracksByAlbums.clear();
    for(const auto &newTrack : qAsConst(d->mNewMusicTracks)) {
        d->mNewTracksByAlbums[newTrack.album()].push_back(newTrack);
    }
}

void DidlParser::searchFinished(UpnpControlAbstractServiceReply *self)
{
    qCDebug(orgKdeElisaUpnp()) << "DidlParser::searchFinished";

    const auto &resultData = self->result();

    bool success = self->success();

    if (!success) {
        qCDebug(orgKdeElisaUpnp()) << "DidlParser::searchFinished" << "error" << self->error();

        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mParentId);

        return;
    }

    QString result = resultData[QStringLiteral("Result")].toString();

    qCDebug(orgKdeElisaUpnp()) << "DidlParser::searchFinished" << "result" << result;

    bool intConvert;
    auto numberReturned = resultData[QStringLiteral("NumberReturned")].toInt(&intConvert);

    qCDebug(orgKdeElisaUpnp()) << "DidlParser::searchFinished" << "NumberReturned" << numberReturned;

    if (!intConvert) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mParentId);

        return;
    }

    auto totalMatches = resultData[QStringLiteral("TotalMatches")].toInt(&intConvert);

    qCDebug(orgKdeElisaUpnp()) << "DidlParser::searchFinished" << "TotalMatches" << totalMatches;

    if (!intConvert) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mParentId);

        return;
    }

    if (totalMatches > numberReturned) {
        search(d->mNewMusicTracks.size() + numberReturned, numberReturned);
    }

    QDomDocument browseDescription;
    browseDescription.setContent(result);

    browseDescription.documentElement();

    auto containerList = browseDescription.elementsByTagName(QStringLiteral("container"));
    for (int containerIndex = 0; containerIndex < containerList.length(); ++containerIndex) {
        const QDomNode &containerNode(containerList.at(containerIndex));
        if (!containerNode.isNull()) {
            decodeContainerNode(containerNode, d->mNewMusicTracks, d->mNewMusicTrackIds);
        }
    }

    auto itemList = browseDescription.elementsByTagName(QStringLiteral("item"));
    for (int itemIndex = 0; itemIndex < itemList.length(); ++itemIndex) {
        const QDomNode &itemNode(itemList.at(itemIndex));
        if (!itemNode.isNull()) {
            decodeAudioTrackNode(itemNode, d->mNewMusicTracks, d->mNewMusicTrackIds);
        }
    }

    groupNewTracksByAlbums();
    d->mIsDataValid = true;
    Q_EMIT isDataValidChanged(d->mParentId);
}

void DidlParser::decodeContainerNode(const QDomNode &containerNode, QHash<QString, DataTypes::UpnpTrackDataType> &newData,
                                     QVector<QString> &newDataIds)
{
    qCDebug(orgKdeElisaUpnp()) << "DidlParser::decodeContainerNode";

    auto allChilds = containerNode.childNodes();

    for(int i = 0; i < allChilds.count(); ++i) {
        const auto &oneChild = allChilds.at(i);
        qCDebug(orgKdeElisaUpnp()) << "DidlParser::decodeContainerNode" << oneChild.nodeName();
    }

    auto parentID = containerNode.toElement().attribute(QStringLiteral("parentID"));
    const auto &id = containerNode.toElement().attribute(QStringLiteral("id"));

    newDataIds.push_back(id);
    auto &childData = newData[id];

    childData[DataTypes::ColumnsRoles::ParentIdRole] = parentID;
    childData[DataTypes::ColumnsRoles::IdRole] = id;

    const QString &childCount = containerNode.toElement().attribute(QStringLiteral("childCount"));
    childData[DataTypes::ColumnsRoles::ChildCountRole] = childCount.toInt();

    const QDomNode &titleNode = containerNode.firstChildElement(QStringLiteral("dc:title"));
    if (!titleNode.isNull()) {
        childData[DataTypes::ColumnsRoles::TitleRole] = titleNode.toElement().text();
    }

    const QDomNode &authorNode = containerNode.firstChildElement(QStringLiteral("upnp:artist"));
    if (!authorNode.isNull()) {
        childData[DataTypes::ColumnsRoles::ArtistRole] = authorNode.toElement().text();
    }

    const QDomNode &resourceNode = containerNode.firstChildElement(QStringLiteral("res"));
    if (!resourceNode.isNull()) {
        childData[DataTypes::ColumnsRoles::ResourceRole] = QUrl::fromUserInput(resourceNode.toElement().text());
    }

    const QDomNode &classNode = containerNode.firstChildElement(QStringLiteral("upnp:class"));
    qCDebug(orgKdeElisaUpnp()) << "DidlParser::decodeContainerNode" << "upnp:class" << classNode.toElement().text();
//    if (classNode.toElement().text().startsWith(QLatin1String("object.container.album.musicAlbum"))) {
//        childData[DataTypes::ElementTypeRole] = QVariant::fromValue(ElisaUtils::Album);
//    } else if (classNode.toElement().text().startsWith(QLatin1String("object.container.person.musicArtist"))) {
//        childData[DataTypes::ElementTypeRole] = QVariant::fromValue(ElisaUtils::Artist);
//    } else if (classNode.toElement().text().startsWith(QLatin1String("object.container"))) {
        childData[DataTypes::ElementTypeRole] = QVariant::fromValue(ElisaUtils::UpnpMediaServer);
        childData[DataTypes::UUIDRole] = d->mDeviceUUID;
//    }

    const QDomNode &albumArtNode = containerNode.firstChildElement(QStringLiteral("upnp:albumArtURI"));
    if (!albumArtNode.isNull()) {
        childData[DataTypes::ColumnsRoles::ImageUrlRole] = QUrl::fromUserInput(albumArtNode.toElement().text());
    }

    qCDebug(orgKdeElisaUpnp()) << "DidlParser::decodeContainerNode" << childData;
}

void DidlParser::decodeAudioTrackNode(const QDomNode &itemNode, QHash<QString, DataTypes::UpnpTrackDataType> &newData,
                                      QVector<QString> &newDataIds)
{
    qCDebug(orgKdeElisaUpnp()) << "DidlParser::decodeAudioTrackNode";

    auto allChilds = itemNode.childNodes();

    for(int i = 0; i < allChilds.count(); ++i) {
        const auto &oneChild = allChilds.at(i);
        qCDebug(orgKdeElisaUpnp()) << "DidlParser::decodeAudioTrackNode" << oneChild.nodeName();
    }

    const QString &parentID = itemNode.toElement().attribute(QStringLiteral("parentID"));
    const QString &id = itemNode.toElement().attribute(QStringLiteral("id"));

    newDataIds.push_back(id);
    auto &childData = newData[id];

    childData[DataTypes::ElementTypeRole] = QVariant::fromValue(ElisaUtils::Track);
    childData[DataTypes::ColumnsRoles::ParentIdRole] = parentID;
    childData[DataTypes::ColumnsRoles::IdRole] = id;

    const QDomNode &titleNode = itemNode.firstChildElement(QStringLiteral("dc:title"));
    if (!titleNode.isNull()) {
        childData[DataTypes::ColumnsRoles::TitleRole] = titleNode.toElement().text();
    }

    const QDomNode &authorNode = itemNode.firstChildElement(QStringLiteral("dc:creator"));
    if (!authorNode.isNull()) {
        childData[DataTypes::ColumnsRoles::ArtistRole] = authorNode.toElement().text();
    }

    const QDomNode &albumAuthorNode = itemNode.firstChildElement(QStringLiteral("upnp:artist"));
    if (!albumAuthorNode.isNull()) {
        childData[DataTypes::ColumnsRoles::AlbumArtistRole] = albumAuthorNode.toElement().text();
    }

    if (childData.albumArtist().isEmpty()) {
        childData[DataTypes::ColumnsRoles::AlbumArtistRole] = childData.artist();
    }

    if (childData.artist().isEmpty()) {
        childData[DataTypes::ColumnsRoles::ArtistRole] = childData.albumArtist();
    }

    const QDomNode &albumNode = itemNode.firstChildElement(QStringLiteral("upnp:album"));
    if (!albumNode.isNull()) {
        childData.setAlbum(albumNode.toElement().text());
    }

    const QDomNode &albumArtNode = itemNode.firstChildElement(QStringLiteral("upnp:albumArtURI"));
    if (!albumArtNode.isNull()) {
        childData[DataTypes::ColumnsRoles::ImageUrlRole] = QUrl::fromUserInput(albumArtNode.toElement().text());
    }

    const QDomNode &resourceNode = itemNode.firstChildElement(QStringLiteral("res"));
    if (!resourceNode.isNull()) {
        childData[DataTypes::ColumnsRoles::ResourceRole] = QUrl::fromUserInput(resourceNode.toElement().text());
        if (resourceNode.attributes().contains(QStringLiteral("duration"))) {
            const QDomNode &durationNode = resourceNode.attributes().namedItem(QStringLiteral("duration"));
            QString durationValue = durationNode.nodeValue();
            if (durationValue.startsWith(QLatin1String("0:"))) {
                durationValue.remove(0, 2);
            }
            if (durationValue.contains(uint('.'))) {
                durationValue = durationValue.split(QLatin1Char('.')).first();
            }

            childData[DataTypes::ColumnsRoles::DurationRole] = QTime::fromString(durationValue, QStringLiteral("mm:ss"));
            if (!childData.duration().isValid()) {
                childData[DataTypes::ColumnsRoles::DurationRole] = QTime::fromString(durationValue, QStringLiteral("hh:mm:ss"));
                if (!childData.duration().isValid()) {
                    childData[DataTypes::ColumnsRoles::DurationRole] = QTime::fromString(durationValue, QStringLiteral("hh:mm:ss.z"));
                }
            }
        }

        const QDomNode &trackNumberNode = itemNode.firstChildElement(QStringLiteral("upnp:originalTrackNumber"));
        if (!trackNumberNode.isNull()) {
            childData[DataTypes::ColumnsRoles::TrackNumberRole] = trackNumberNode.toElement().text().toInt();
        }

        if (resourceNode.attributes().contains(QStringLiteral("artist"))) {
            const QDomNode &artistNode = resourceNode.attributes().namedItem(QStringLiteral("artist"));
            childData[DataTypes::ColumnsRoles::ArtistRole] = artistNode.nodeValue();
        }
    }

    qCDebug(orgKdeElisaUpnp()) << "DidlParser::decodeAudioTrackNode" << childData;
}


#include "moc_didlparser.cpp"

