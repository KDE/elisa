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

#include "upnpalbummodel.h"

#include <upnpcontrolcontentdirectory.h>

#include <QtXml/QDomDocument>

#include <QtCore/QUrl>
#include <QtCore/QDebug>

class UpnpAlbumModelPrivate
{
public:

    UpnpControlContentDirectory *mContentDirectory = nullptr;

    QString mBrowseFlag;

    QString mFilter;

    QString mSortCriteria;

    quintptr mLastInternalId = 1;

    QHash<QString, quintptr> mUpnpIds;

    QHash<quintptr, QVector<quintptr> > mChilds;

    QHash<quintptr, QHash<UpnpAlbumModel::ColumnsRoles, QVariant> > mData;

    int mCurrentUpdateId = -1;

    bool mUseLocalIcons = false;

    QAbstractItemModel *mUpnpModel = nullptr;

};

UpnpAlbumModel::UpnpAlbumModel(QObject *parent) : QAbstractItemModel(parent), d(new UpnpAlbumModelPrivate)
{
    d->mUpnpIds[QStringLiteral("0")] = d->mLastInternalId;

    d->mChilds[d->mLastInternalId] = QVector<quintptr>();

    d->mData[d->mLastInternalId] = QHash<UpnpAlbumModel::ColumnsRoles, QVariant>();
    d->mData[d->mLastInternalId][ColumnsRoles::IdRole] = QStringLiteral("0");
}

UpnpAlbumModel::~UpnpAlbumModel()
{
    delete d;
}

int UpnpAlbumModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 1;
    }

    if (!d->mChilds.contains(parent.internalId())) {
        return 0;
    }

    return d->mChilds[parent.internalId()].size();
}

QHash<int, QByteArray> UpnpAlbumModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::ItemClassRole)] = "itemClass";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsPlayingRole)] = "isPlaying";

    return roles;
}

bool UpnpAlbumModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return false;
    }

    auto parentInternalId = parent.internalId();

    if (!d->mChilds.contains(parentInternalId)) {
        return true;
    }

    return d->mChilds[parentInternalId].isEmpty();
}

void UpnpAlbumModel::fetchMore(const QModelIndex &parent)
{
    qDebug() << "UpnpAlbumModel::fetchMore";

    if (!d->mContentDirectory) {
        return;
    }

    if (!parent.isValid()) {
        return;
    }

    auto parentInternalId = parent.internalId();

    if (!d->mData.contains(parentInternalId)) {
        return;
    }

    if (!d->mData[parentInternalId].contains(ColumnsRoles::IdRole)) {
        return;
    }

    qDebug() << "UpnpAlbumModel::fetchMore" << "OK";

    if (d->mData[parentInternalId][ColumnsRoles::IdRole].toString() == QStringLiteral("0")) {
        d->mContentDirectory->search(d->mData[parentInternalId][ColumnsRoles::IdRole].toString(),
                QStringLiteral("upnp:class derivedfrom \"object.container.album.musicAlbum\""), d->mFilter, 0, 0, d->mSortCriteria);
    } else {
        d->mContentDirectory->browse(d->mData[parentInternalId][ColumnsRoles::IdRole].toString(), d->mBrowseFlag, d->mFilter, 0, 0, d->mSortCriteria);
    }
}

Qt::ItemFlags UpnpAlbumModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant UpnpAlbumModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.column() != 0) {
        return QVariant();
    }

    if (index.row() < 0) {
        return QVariant();
    }

    if (!d->mData.contains(index.internalId())) {
        return QVariant();
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(role)
    {
    case ColumnsRoles::TitleRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::DurationRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::CreatorRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::ArtistRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::AlbumRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::RatingRole:
        return 0;
    case ColumnsRoles::ImageRole:
        switch (d->mData[index.internalId()][ColumnsRoles::ItemClassRole].value<int>())
        {
        case UpnpAlbumModel::Album:
            if (!d->mData[index.internalId()][ColumnsRoles::ImageRole].toString().isEmpty()) {
                return d->mData[index.internalId()][ColumnsRoles::ImageRole].toUrl();
            } else {
                if (d->mUseLocalIcons) {
                    return QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
                } else {
                    return QUrl(QStringLiteral("image://icon/media-optical-audio"));
                }
            }
        case UpnpAlbumModel::Container:
            if (d->mUseLocalIcons) {
                return QUrl(QStringLiteral("qrc:/folder.svg"));
            } else {
                return QUrl(QStringLiteral("image://icon/folder"));
            }
        case UpnpAlbumModel::AudioTrack:
            return data(index.parent(), role);
        }
    case ColumnsRoles::ResourceRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::ItemClassRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::CountRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::IdRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::ParentIdRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::IsPlayingRole:
        return d->mData[index.internalId()][convertedRole];
    }

    return QVariant();
}

QModelIndex UpnpAlbumModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(0, 0, 1);
    }

    if (!d->mChilds.contains(parent.internalId())) {
        return {};
    }

    if (row < 0 && row >= d->mChilds[parent.internalId()].size()) {
        return {};
    }

    if (column != 0) {
        return {};
    }

    return createIndex(row, column, d->mChilds[parent.internalId()][row]);
}

QModelIndex UpnpAlbumModel::parent(const QModelIndex &child) const
{
    // child is valid
    if (!child.isValid()) {
        return {};
    }

    // data knows child internal id
    if (!d->mData.contains(child.internalId())) {
        return {};
    }

    const auto &childData = d->mData[child.internalId()];

    // child data has upnp id of parent
    if (!childData.contains(ColumnsRoles::ParentIdRole)) {
        return {};
    }

    const auto &parentStringId = childData[ColumnsRoles::ParentIdRole].toString();

    // upnp ids has the internal id of parent
    if (!d->mUpnpIds.contains(parentStringId)) {
        return {};
    }

    auto parentInternalId = d->mUpnpIds[parentStringId];

    // special case if we are already at top of model
    if (parentStringId == QStringLiteral("0")) {
        return createIndex(0, 0, parentInternalId);
    }

    // data knows parent internal id
    if (!d->mData.contains(parentInternalId)) {
        return {};
    }

    const auto &parentData = d->mData[parentInternalId];

    // parent data has upnp id of parent
    if (!parentData.contains(ColumnsRoles::ParentIdRole)) {
        return {};
    }

    const auto &grandParentStringId = parentData[ColumnsRoles::ParentIdRole].toString();

    // upnp ids has the internal id of grand parent
    if (!d->mUpnpIds.contains(grandParentStringId)) {
        return {};
    }

    auto grandParentInternalId = d->mUpnpIds[grandParentStringId];

    // childs of grand parent are known
    if (!d->mChilds.contains(grandParentInternalId)) {
        return {};
    }

    const auto &allUncles = d->mChilds[grandParentInternalId];

    // look for my parent
    for (int i = 0; i < allUncles.size(); ++i) {
        if (allUncles[i] == parentInternalId) {
            return createIndex(i, 0, parentInternalId);
        }
    }

    // I have no parent
    return {};
}

int UpnpAlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QString UpnpAlbumModel::objectIdByIndex(const QModelIndex &index) const
{
    return data(index, ColumnsRoles::IdRole).toString();
}

QVariant UpnpAlbumModel::getUrl(const QModelIndex &index) const
{
    return data(index, ColumnsRoles::ResourceRole);
}

QModelIndex UpnpAlbumModel::indexFromId(const QString &id) const
{
    if (!d->mUpnpIds.contains(id)) {
        return {};
    }

    return indexFromInternalId(d->mUpnpIds[id]);
}

UpnpControlContentDirectory *UpnpAlbumModel::contentDirectory() const
{
    return d->mContentDirectory;
}

const QString &UpnpAlbumModel::browseFlag() const
{
    return d->mBrowseFlag;
}

void UpnpAlbumModel::setBrowseFlag(const QString &flag)
{
    d->mBrowseFlag = flag;
    Q_EMIT browseFlagChanged();
}

const QString &UpnpAlbumModel::filter() const
{
    return d->mFilter;
}

void UpnpAlbumModel::setFilter(const QString &flag)
{
    d->mFilter = flag;
    Q_EMIT filterChanged();
}

const QString &UpnpAlbumModel::sortCriteria() const
{
    return d->mSortCriteria;
}

void UpnpAlbumModel::setSortCriteria(const QString &criteria)
{
    d->mSortCriteria = criteria;
    Q_EMIT sortCriteriaChanged();
}

void UpnpAlbumModel::setContentDirectory(UpnpControlContentDirectory *directory)
{
    if (directory) {
        beginResetModel();
    }

    if (d->mContentDirectory) {
        disconnect(d->mContentDirectory, &UpnpControlContentDirectory::browseFinished, this, &UpnpAlbumModel::browseFinished);
        disconnect(d->mContentDirectory, &UpnpControlContentDirectory::searchFinished, this, &UpnpAlbumModel::browseFinished);
    }

    d->mContentDirectory = directory;

    if (!d->mContentDirectory) {
        Q_EMIT contentDirectoryChanged();
        return;
    }

    connect(d->mContentDirectory, &UpnpControlContentDirectory::browseFinished, this, &UpnpAlbumModel::browseFinished);
    connect(d->mContentDirectory, &UpnpControlContentDirectory::searchFinished, this, &UpnpAlbumModel::browseFinished);
    endResetModel();

    Q_EMIT contentDirectoryChanged();
}

void UpnpAlbumModel::scanAll()
{
    if (!d->mUpnpModel) {
        return;
    }

    auto rootIndex = d->mUpnpModel->index(0, 0);

    for (int i = 0; i < d->mUpnpModel->rowCount(rootIndex); ++i) {
        d->mUpnpModel->fetchMore(d->mUpnpModel->index(i, 0, rootIndex));

        qDebug() << "UpnpAlbumModel::scanAll" << (d->mUpnpModel->canFetchMore(d->mUpnpModel->index(i, 0, rootIndex)) ? "true" : "false");
    }
}

void UpnpAlbumModel::browseFinished(const QString &result, int numberReturned, int totalMatches, int systemUpdateID, bool success)
{
    Q_UNUSED(numberReturned)
    Q_UNUSED(totalMatches)
    Q_UNUSED(systemUpdateID)

    qDebug() << "UpnpAlbumModel::browseFinished" << numberReturned;

    if (!success) {
        qDebug() << "UpnpAlbumModel::browseFinished" << "in error";
        return;
    }

    if (d->mCurrentUpdateId == -1 || d->mCurrentUpdateId != systemUpdateID) {
        d->mCurrentUpdateId = systemUpdateID;
    }

    QDomDocument browseDescription;
    browseDescription.setContent(result);

    browseDescription.documentElement();

    QList<quintptr> newDataIds;
    QHash<QString, quintptr> newIdMappings;
    decltype(d->mData) newData;

    auto containerList = browseDescription.elementsByTagName(QStringLiteral("container"));
    qDebug() << "UpnpAlbumModel::browseFinished" << containerList.length();
    for (int containerIndex = 0; containerIndex < containerList.length(); ++containerIndex) {
        const QDomNode &containerNode(containerList.at(containerIndex));
        if (!containerNode.isNull()) {
            decodeContainerNode(containerNode, newDataIds, newIdMappings, newData);
        }
    }

    auto itemList = browseDescription.elementsByTagName(QStringLiteral("item"));
    for (int itemIndex = 0; itemIndex < itemList.length(); ++itemIndex) {
        const QDomNode &itemNode(itemList.at(itemIndex));
        if (!itemNode.isNull()) {
            decodeAudioTrackNode(itemNode, newDataIds, newIdMappings, newData);
        }
    }

    qDebug() << "UpnpAlbumModel::browseFinished" << "decoding finished";
    if (!newDataIds.isEmpty()) {
        QString parentId = newData[newDataIds.first()][ColumnsRoles::ParentIdRole].toString();

        if (!d->mUpnpIds.contains(parentId)) {
            return;
        }

        auto parentInternalId = d->mUpnpIds[parentId];

        if (!d->mData.contains(parentInternalId)) {
            return;
        }

        auto &childData = d->mChilds[parentInternalId];

        if (!childData.isEmpty() && d->mCurrentUpdateId == systemUpdateID) {
            return;
        }

        if (!childData.isEmpty()) {
            beginRemoveRows(indexFromInternalId(parentInternalId), 0, childData.size() - 1);
            d->mChilds[parentInternalId].clear();
            endRemoveRows();
        }

        qDebug() << "UpnpAlbumModel::browseFinished" << newDataIds.size();
        beginInsertRows(indexFromInternalId(parentInternalId), 0, newDataIds.size() - 1);
        for (auto childInternalId : newDataIds) {
            d->mChilds[parentInternalId].push_back(childInternalId);
            d->mUpnpIds[newData[childInternalId][ColumnsRoles::IdRole].toString()] = childInternalId;
            d->mData[childInternalId] = newData[childInternalId];
        }
        endInsertRows();
    }
}

QModelIndex UpnpAlbumModel::indexFromInternalId(quintptr internalId) const
{
    if (internalId == 1) {
        return createIndex(0, 0, internalId);
    }

    // data knows child internal id
    if (!d->mData.contains(internalId)) {
        return {};
    }

    const auto &childData = d->mData[internalId];

    // child data has upnp id of parent
    if (!childData.contains(ColumnsRoles::ParentIdRole)) {
        return {};
    }

    const auto &parentStringId = childData[ColumnsRoles::ParentIdRole].toString();

    // upnp ids has the internal id of parent
    if (!d->mUpnpIds.contains(parentStringId)) {
        return {};
    }

    auto parentInternalId = d->mUpnpIds[parentStringId];

    // childs of parent are known
    if (!d->mChilds.contains(parentInternalId)) {
        return {};
    }

    const auto &allUncles = d->mChilds[parentInternalId];

    // look for my parent
    for (int i = 0; i < allUncles.size(); ++i) {
        if (allUncles[i] == internalId) {
            return createIndex(i, 0, internalId);
        }
    }

    // I have no parent
    return {};
}

void UpnpAlbumModel::decodeContainerNode(const QDomNode &containerNode, QList<quintptr> &newDataIds,
                                         QHash<QString, quintptr> newIdMappings,
                                         QHash<quintptr, QHash<ColumnsRoles, QVariant> > &newData)
{
    auto parentID = containerNode.toElement().attribute(QStringLiteral("parentID"));
    const auto &id = containerNode.toElement().attribute(QStringLiteral("id"));

    if (!d->mUpnpIds.contains(parentID)) {
        qDebug() << "UpnpAlbumModel::browseFinished" << "unknown parent id" << parentID << d->mUpnpIds.keys();

        parentID = QStringLiteral("0");
    }

    ++(d->mLastInternalId);
    newDataIds.push_back(d->mLastInternalId);
    newIdMappings[id] = d->mLastInternalId;
    auto &chilData = newData[d->mLastInternalId];

    chilData[ParentIdRole] = parentID;
    chilData[IdRole] = id;

    const QString &childCount = containerNode.toElement().attribute(QStringLiteral("childCount"));
    chilData[ColumnsRoles::CountRole] = childCount;

    const QDomNode &titleNode = containerNode.firstChildElement(QStringLiteral("dc:title"));
    if (!titleNode.isNull()) {
        chilData[ColumnsRoles::TitleRole] = titleNode.toElement().text();
    }

    const QDomNode &authorNode = containerNode.firstChildElement(QStringLiteral("upnp:artist"));
    if (!authorNode.isNull()) {
        chilData[ColumnsRoles::ArtistRole] = authorNode.toElement().text();
    }

    const QDomNode &albumNode = containerNode.firstChildElement(QStringLiteral("upnp:album"));
    if (!albumNode.isNull()) {
        chilData[ColumnsRoles::AlbumRole] = albumNode.toElement().text();
    }

    const QDomNode &resourceNode = containerNode.firstChildElement(QStringLiteral("res"));
    if (!resourceNode.isNull()) {
        chilData[ColumnsRoles::ResourceRole] = resourceNode.toElement().text();
    }

    const QDomNode &classNode = containerNode.firstChildElement(QStringLiteral("upnp:class"));
    if (classNode.toElement().text().startsWith(QStringLiteral("object.item.audioItem"))) {
        chilData[ColumnsRoles::ItemClassRole] = UpnpAlbumModel::AudioTrack;
    } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container.album"))) {
        chilData[ColumnsRoles::ItemClassRole] = UpnpAlbumModel::Album;
    } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container"))) {
        chilData[ColumnsRoles::ItemClassRole] = UpnpAlbumModel::Container;
    }

    const QDomNode &albumArtNode = containerNode.firstChildElement(QStringLiteral("upnp:albumArtURI"));
    if (!albumArtNode.isNull()) {
        chilData[ColumnsRoles::ImageRole] = albumArtNode.toElement().text();
    }

    const QDomNode &storageUsedNode = containerNode.firstChildElement(QStringLiteral("upnp:storageUsed"));
    if (!storageUsedNode.isNull()) {
    }
}

void UpnpAlbumModel::decodeAudioTrackNode(const QDomNode &itemNode, QList<quintptr> &newDataIds, QHash<QString, quintptr> newIdMappings,
                                          QHash<quintptr, QHash<ColumnsRoles, QVariant> > &newData)
{
    const QString &parentID = itemNode.toElement().attribute(QStringLiteral("parentID"));
    const QString &id = itemNode.toElement().attribute(QStringLiteral("id"));

    if (!d->mUpnpIds.contains(parentID)) {
        qDebug() << "UpnpAlbumModel::browseFinished" << "unknown parent id";
        return;
    }

    ++(d->mLastInternalId);
    newDataIds.push_back(d->mLastInternalId);
    newIdMappings[id] = d->mLastInternalId;
    auto &chilData = newData[d->mLastInternalId];

    chilData[ParentIdRole] = parentID;
    chilData[IdRole] = id;

    const QString &childCount = itemNode.toElement().attribute(QStringLiteral("childCount"));
    chilData[ColumnsRoles::CountRole] = childCount;

    const QDomNode &titleNode = itemNode.firstChildElement(QStringLiteral("dc:title"));
    if (!titleNode.isNull()) {
        chilData[ColumnsRoles::TitleRole] = titleNode.toElement().text();
    }

    const QDomNode &authorNode = itemNode.firstChildElement(QStringLiteral("upnp:artist"));
    if (!authorNode.isNull()) {
        chilData[ColumnsRoles::ArtistRole] = authorNode.toElement().text();
    }

    const QDomNode &albumNode = itemNode.firstChildElement(QStringLiteral("upnp:album"));
    if (!albumNode.isNull()) {
        chilData[ColumnsRoles::AlbumRole] = albumNode.toElement().text();
    }

    const QDomNode &resourceNode = itemNode.firstChildElement(QStringLiteral("res"));
    if (!resourceNode.isNull()) {
        chilData[ColumnsRoles::ResourceRole] = resourceNode.toElement().text();
        if (resourceNode.attributes().contains(QStringLiteral("duration"))) {
            const QDomNode &durationNode = resourceNode.attributes().namedItem(QStringLiteral("duration"));
            QString durationValue = durationNode.nodeValue();
            if (durationValue.startsWith(QStringLiteral("0:"))) {
                durationValue = durationValue.mid(2);
            }
            if (durationValue.contains(uint('.'))) {
                durationValue = durationValue.split(QStringLiteral(".")).first();
            }

            chilData[ColumnsRoles::DurationRole] = durationValue;
        }
        if (resourceNode.attributes().contains(QStringLiteral("artist"))) {
            const QDomNode &artistNode = resourceNode.attributes().namedItem(QStringLiteral("artist"));
            chilData[ColumnsRoles::ArtistRole] = artistNode.nodeValue();
        }
    }

    const QDomNode &classNode = itemNode.firstChildElement(QStringLiteral("upnp:class"));
    if (!classNode.isNull()) {
        if (classNode.toElement().text().startsWith(QStringLiteral("object.item.audioItem"))) {
            chilData[ColumnsRoles::ItemClassRole] = UpnpAlbumModel::AudioTrack;
        } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container.album"))) {
            chilData[ColumnsRoles::ItemClassRole] = UpnpAlbumModel::Album;
        } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container"))) {
            chilData[ColumnsRoles::ItemClassRole] = UpnpAlbumModel::Container;
        }
    }

    const QDomNode &storageUsedNode = itemNode.firstChildElement(QStringLiteral("upnp:storageUsed"));
    if (!storageUsedNode.isNull()) {
    }
}


#include "moc_upnpalbummodel.cpp"
