/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "upnpcontentdirectorymodel.h"
#include "upnpcontrolcontentdirectory.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include <QHash>
#include <QString>
#include <QList>
#include <QDebug>
#include <QPointer>
#include <QUrl>

class UpnpContentDirectoryModelPrivate
{
public:

    UpnpControlContentDirectory *mContentDirectory;

    QString mBrowseFlag;

    QString mFilter;

    QString mSortCriteria;

    quintptr mLastInternalId;

    QHash<QString, quintptr> mUpnpIds;

    QHash<quintptr, QVector<quintptr> > mChilds;

    QHash<quintptr, QHash<UpnpContentDirectoryModel::ColumnsRoles, QVariant> > mData;

    int mCurrentUpdateId;

    bool mUseLocalIcons = false;

};

UpnpContentDirectoryModel::UpnpContentDirectoryModel(QObject *parent)
    : QAbstractItemModel(parent), d(new UpnpContentDirectoryModelPrivate)
{
    d->mContentDirectory = nullptr;

    d->mLastInternalId = 1;

    d->mUpnpIds[QStringLiteral("0")] = d->mLastInternalId;

    d->mChilds[d->mLastInternalId] = QVector<quintptr>();

    d->mData[d->mLastInternalId] = QHash<UpnpContentDirectoryModel::ColumnsRoles, QVariant>();
    d->mData[d->mLastInternalId][ColumnsRoles::IdRole] = QStringLiteral("0");

    d->mCurrentUpdateId = -1;
}

UpnpContentDirectoryModel::~UpnpContentDirectoryModel()
{
}

int UpnpContentDirectoryModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 1;
    }

    if (!d->mChilds.contains(parent.internalId())) {
        return 0;
    }

    return d->mChilds[parent.internalId()].size();
}

QHash<int, QByteArray> UpnpContentDirectoryModel::roleNames() const
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

Qt::ItemFlags UpnpContentDirectoryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant UpnpContentDirectoryModel::data(const QModelIndex &index, int role) const
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
        case UpnpContentDirectoryModel::Album:
            if (!d->mData[index.internalId()][ColumnsRoles::ImageRole].toString().isEmpty()) {
                return d->mData[index.internalId()][ColumnsRoles::ImageRole].toUrl();
            } else {
                if (d->mUseLocalIcons) {
                    return QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
                } else {
                    return QUrl(QStringLiteral("image://icon/media-optical-audio"));
                }
            }
        case UpnpContentDirectoryModel::Container:
            if (d->mUseLocalIcons) {
                return QUrl(QStringLiteral("qrc:/folder.svg"));
            } else {
                return QUrl(QStringLiteral("image://icon/folder"));
            }
        case UpnpContentDirectoryModel::AudioTrack:
            return data(index.parent(), role);
        }
        break;
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

QModelIndex UpnpContentDirectoryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(0, 0, 1);
    }

    if (!d->mChilds.contains(parent.internalId())) {
        return QModelIndex();
    }

    if (row < 0 && row >= d->mChilds[parent.internalId()].size()) {
        return QModelIndex();
    }

    if (column != 0) {
        return QModelIndex();
    }

    return createIndex(row, column, d->mChilds[parent.internalId()][row]);
}

QModelIndex UpnpContentDirectoryModel::parent(const QModelIndex &child) const
{
    // child is valid
    if (!child.isValid()) {
        return QModelIndex();
    }

    // data knows child internal id
    if (!d->mData.contains(child.internalId())) {
        return QModelIndex();
    }

    const auto &childData = d->mData[child.internalId()];

    // child data has upnp id of parent
    if (!childData.contains(ColumnsRoles::ParentIdRole)) {
        return QModelIndex();
    }

    const auto &parentStringId = childData[ColumnsRoles::ParentIdRole].toString();

    // upnp ids has the internal id of parent
    if (!d->mUpnpIds.contains(parentStringId)) {
        return QModelIndex();
    }

    auto parentInternalId = d->mUpnpIds[parentStringId];

    // special case if we are already at top of model
    if (parentStringId == QStringLiteral("0")) {
        return createIndex(0, 0, parentInternalId);
    }

    // data knows parent internal id
    if (!d->mData.contains(parentInternalId)) {
        return QModelIndex();
    }

    const auto &parentData = d->mData[parentInternalId];

    // parent data has upnp id of parent
    if (!parentData.contains(ColumnsRoles::ParentIdRole)) {
        return QModelIndex();
    }

    const auto &grandParentStringId = parentData[ColumnsRoles::ParentIdRole].toString();

    // upnp ids has the internal id of grand parent
    if (!d->mUpnpIds.contains(grandParentStringId)) {
        return QModelIndex();
    }

    auto grandParentInternalId = d->mUpnpIds[grandParentStringId];

    // childs of grand parent are known
    if (!d->mChilds.contains(grandParentInternalId)) {
        return QModelIndex();
    }

    const auto &allUncles = d->mChilds[grandParentInternalId];

    // look for my parent
    for (int i = 0; i < allUncles.size(); ++i) {
        if (allUncles[i] == parentInternalId) {
            return createIndex(i, 0, parentInternalId);
        }
    }

    // I have no parent
    return QModelIndex();
}

int UpnpContentDirectoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

bool UpnpContentDirectoryModel::canFetchMore(const QModelIndex &parent) const
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

void UpnpContentDirectoryModel::fetchMore(const QModelIndex &parent)
{
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

    if (d->mData[parentInternalId][ColumnsRoles::IdRole].toString() == QStringLiteral("0")) {
        d->mContentDirectory->search(d->mData[parentInternalId][ColumnsRoles::IdRole].toString(),
                QStringLiteral("upnp:class derivedfrom \"object.container.album\""), d->mFilter, 0, 0, d->mSortCriteria);
    } else {
        d->mContentDirectory->browse(d->mData[parentInternalId][ColumnsRoles::IdRole].toString(), d->mBrowseFlag, d->mFilter, 0, 0, d->mSortCriteria);
    }
}

const QString &UpnpContentDirectoryModel::browseFlag() const
{
    return d->mBrowseFlag;
}

void UpnpContentDirectoryModel::setBrowseFlag(const QString &flag)
{
    d->mBrowseFlag = flag;
    Q_EMIT browseFlagChanged();
}

const QString &UpnpContentDirectoryModel::filter() const
{
    return d->mFilter;
}

void UpnpContentDirectoryModel::setFilter(const QString &flag)
{
    d->mFilter = flag;
    Q_EMIT filterChanged();
}

const QString &UpnpContentDirectoryModel::sortCriteria() const
{
    return d->mSortCriteria;
}

void UpnpContentDirectoryModel::setSortCriteria(const QString &criteria)
{
    d->mSortCriteria = criteria;
    Q_EMIT sortCriteriaChanged();
}

UpnpControlContentDirectory *UpnpContentDirectoryModel::contentDirectory() const
{
    return d->mContentDirectory;
}

void UpnpContentDirectoryModel::setContentDirectory(UpnpControlContentDirectory *directory)
{
    if (directory) {
        beginResetModel();
    }

    if (d->mContentDirectory) {
        //disconnect(d->mContentDirectory, &UpnpControlContentDirectory::browseFinished, this, &UpnpContentDirectoryModel::browseFinished);
        //disconnect(d->mContentDirectory, &UpnpControlContentDirectory::searchFinished, this, &UpnpContentDirectoryModel::browseFinished);
    }

    d->mContentDirectory = directory;

    if (!d->mContentDirectory) {
        Q_EMIT contentDirectoryChanged();
        return;
    }

    //connect(d->mContentDirectory, &UpnpControlContentDirectory::browseFinished, this, &UpnpContentDirectoryModel::browseFinished);
    //connect(d->mContentDirectory, &UpnpControlContentDirectory::searchFinished, this, &UpnpContentDirectoryModel::browseFinished);
    endResetModel();

    Q_EMIT contentDirectoryChanged();
}

bool UpnpContentDirectoryModel::useLocalIcons() const
{
    return d->mUseLocalIcons;
}

void UpnpContentDirectoryModel::setUseLocalIcons(bool value)
{
    d->mUseLocalIcons = value;
    Q_EMIT useLocalIconsChanged();
}

QString UpnpContentDirectoryModel::objectIdByIndex(const QModelIndex &index) const
{
    return data(index, ColumnsRoles::IdRole).toString();
}

QVariant UpnpContentDirectoryModel::getUrl(const QModelIndex &index) const
{
    return data(index, ColumnsRoles::ResourceRole);
}

QModelIndex UpnpContentDirectoryModel::indexFromId(const QString &id) const
{
    if (!d->mUpnpIds.contains(id)) {
        return QModelIndex();
    }

    return indexFromInternalId(d->mUpnpIds[id]);
}

void UpnpContentDirectoryModel::browseFinished(const QString &result, int numberReturned, int totalMatches, int systemUpdateID, bool success)
{
    Q_UNUSED(numberReturned)
    Q_UNUSED(totalMatches)
    Q_UNUSED(systemUpdateID)

    qDebug() << "UpnpContentDirectoryModel::browseFinished" << numberReturned;

    if (!success) {
        qDebug() << "UpnpContentDirectoryModel::browseFinished" << "in error";
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
    qDebug() << "UpnpContentDirectoryModel::browseFinished" << containerList.length();
    for (int containerIndex = 0; containerIndex < containerList.length(); ++containerIndex) {
        const QDomNode &containerNode(containerList.at(containerIndex));
        if (!containerNode.isNull()) {
            const QString &parentID = containerNode.toElement().attribute(QStringLiteral("parentID"));
            const QString &id = containerNode.toElement().attribute(QStringLiteral("id"));

            if (!d->mUpnpIds.contains(parentID)) {
                qDebug() << "UpnpContentDirectoryModel::browseFinished" << "unknown parent id" << parentID << d->mUpnpIds.keys();
                return;
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
                chilData[ColumnsRoles::ItemClassRole] = UpnpContentDirectoryModel::AudioTrack;
            } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container.album"))) {
                chilData[ColumnsRoles::ItemClassRole] = UpnpContentDirectoryModel::Album;
            } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container"))) {
                chilData[ColumnsRoles::ItemClassRole] = UpnpContentDirectoryModel::Container;
            }

            const QDomNode &albumArtNode = containerNode.firstChildElement(QStringLiteral("upnp:albumArtURI"));
            if (!albumArtNode.isNull()) {
                chilData[ColumnsRoles::ImageRole] = albumArtNode.toElement().text();
            }

            const QDomNode &storageUsedNode = containerNode.firstChildElement(QStringLiteral("upnp:storageUsed"));
            if (!storageUsedNode.isNull()) {
            }
        }
    }

    auto itemList = browseDescription.elementsByTagName(QStringLiteral("item"));
    for (int itemIndex = 0; itemIndex < itemList.length(); ++itemIndex) {
        const QDomNode &itemNode(itemList.at(itemIndex));
        if (!itemNode.isNull()) {
            const QString &parentID = itemNode.toElement().attribute(QStringLiteral("parentID"));
            const QString &id = itemNode.toElement().attribute(QStringLiteral("id"));

            if (!d->mUpnpIds.contains(parentID)) {
                qDebug() << "UpnpContentDirectoryModel::browseFinished" << "unknown parent id";
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
                    chilData[ColumnsRoles::ItemClassRole] = UpnpContentDirectoryModel::AudioTrack;
                } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container.album"))) {
                    chilData[ColumnsRoles::ItemClassRole] = UpnpContentDirectoryModel::Album;
                } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container"))) {
                    chilData[ColumnsRoles::ItemClassRole] = UpnpContentDirectoryModel::Container;
                }
            }

            const QDomNode &storageUsedNode = itemNode.firstChildElement(QStringLiteral("upnp:storageUsed"));
            if (!storageUsedNode.isNull()) {
            }
        }
    }

    qDebug() << "UpnpContentDirectoryModel::browseFinished" << "decoding finished";
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

        qDebug() << "UpnpContentDirectoryModel::browseFinished" << newDataIds.size();
        beginInsertRows(indexFromInternalId(parentInternalId), 0, newDataIds.size() - 1);
        for (auto childInternalId : const_cast<const decltype(newDataIds)&>(newDataIds)) {
            d->mChilds[parentInternalId].push_back(childInternalId);
            d->mUpnpIds[newData[childInternalId][ColumnsRoles::IdRole].toString()] = childInternalId;
            d->mData[childInternalId] = newData[childInternalId];
        }
        endInsertRows();
    }
}

QModelIndex UpnpContentDirectoryModel::indexFromInternalId(quintptr internalId) const
{
    if (internalId == 1) {
        return createIndex(0, 0, internalId);
    }

    // data knows child internal id
    if (!d->mData.contains(internalId)) {
        return QModelIndex();
    }

    const auto &childData = d->mData[internalId];

    // child data has upnp id of parent
    if (!childData.contains(ColumnsRoles::ParentIdRole)) {
        return QModelIndex();
    }

    const auto &parentStringId = childData[ColumnsRoles::ParentIdRole].toString();

    // upnp ids has the internal id of parent
    if (!d->mUpnpIds.contains(parentStringId)) {
        return QModelIndex();
    }

    auto parentInternalId = d->mUpnpIds[parentStringId];

    // childs of parent are known
    if (!d->mChilds.contains(parentInternalId)) {
        return QModelIndex();
    }

    const auto &allUncles = d->mChilds[parentInternalId];

    // look for my parent
    for (int i = 0; i < allUncles.size(); ++i) {
        if (allUncles[i] == internalId) {
            return createIndex(i, 0, internalId);
        }
    }

    // I have no parent
    return QModelIndex();
}

#include "moc_upnpcontentdirectorymodel.cpp"
