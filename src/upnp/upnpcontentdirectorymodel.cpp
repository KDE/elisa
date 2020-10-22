/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "upnpcontentdirectorymodel.h"

#include "musiclistenersmanager.h"

#include "upnpLogging.h"

#include "didlparser.h"
#include "upnpcontrolcontentdirectory.h"
#include "upnpdiscoverallmusic.h"

#include <UpnpDeviceDescription>
#include <UpnpServiceDescription>
#include <UpnpActionDescription>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include <QHash>
#include <QString>
#include <QList>
#include <QLoggingCategory>
#include <QPointer>
#include <QUrl>

class UpnpContentDirectoryModelPrivate
{
public:

    UpnpControlContentDirectory *mContentDirectory;

    DidlParser mDidlParser;

    QString mParentId = QStringLiteral("0");

    QString mBrowseFlag = QStringLiteral("BrowseDirectChildren");

    QString mFilter = QStringLiteral("*");

    QString mSortCriteria;

    quintptr mLastInternalId;

    QHash<QString, quintptr> mUpnpIds;

    QHash<quintptr, QVector<quintptr> > mChilds;

    QHash<quintptr, DataTypes::UpnpTrackDataType> mAllTrackData;

    int mCurrentUpdateId;

    bool mUseLocalIcons = false;

    bool mIsBusy = false;

};

UpnpContentDirectoryModel::UpnpContentDirectoryModel(QObject *parent)
    : QAbstractItemModel(parent), d(new UpnpContentDirectoryModelPrivate)
{
    d->mContentDirectory = nullptr;

    connect(&d->mDidlParser, &DidlParser::isDataValidChanged, this, &UpnpContentDirectoryModel::contentChanged);
}

UpnpContentDirectoryModel::~UpnpContentDirectoryModel()
= default;

int UpnpContentDirectoryModel::rowCount(const QModelIndex &parent) const
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::rowCount" << parent;

    int result = 0;

    auto currentInternalId = parent.internalId();

    if (!parent.isValid()) {
        currentInternalId = d->mUpnpIds[parentId()];
    }

    if (!d->mChilds.contains(currentInternalId)) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::rowCount" << parent << currentInternalId << "unknown child" << result;

        if (d->mAllTrackData.contains(currentInternalId)) {
            result = d->mAllTrackData[currentInternalId][DataTypes::ChildCountRole].toInt();

            qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::rowCount" << parent << currentInternalId << result;
        }

        return result;
    }

    result = d->mChilds[currentInternalId].size();

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::rowCount" << parent << currentInternalId << result;

    return result;
}

QHash<int, QByteArray> UpnpContentDirectoryModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(DataTypes::TitleRole)] = "title";
    roles[static_cast<int>(DataTypes::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(DataTypes::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(DataTypes::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(DataTypes::ElementTypeRole)] = "dataType";
    roles[static_cast<int>(DataTypes::ResourceRole)] = "url";

    roles[static_cast<int>(DataTypes::ArtistRole)] = "artist";
    roles[static_cast<int>(DataTypes::AllArtistsRole)] = "allArtists";
    roles[static_cast<int>(DataTypes::HighestTrackRating)] = "highestTrackRating";
    roles[static_cast<int>(DataTypes::GenreRole)] = "genre";

    roles[static_cast<int>(DataTypes::AlbumRole)] = "album";
    roles[static_cast<int>(DataTypes::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(DataTypes::DurationRole)] = "duration";
    roles[static_cast<int>(DataTypes::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(DataTypes::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(DataTypes::RatingRole)] = "rating";
    roles[static_cast<int>(DataTypes::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";
    roles[static_cast<int>(DataTypes::FullDataRole)] = "fullData";
    roles[static_cast<int>(DataTypes::HasModelChildrenRole)] = "hasModelChildren";

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
    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::data" << index << role;

    QVariant result;

    if (!index.isValid()) {
        return result;
    }

    if (index.column() != 0) {
        return result;
    }

    if (index.row() < 0) {
        return result;
    }

    if (!d->mAllTrackData.contains(index.internalId())) {
        return result;
    }

    auto convertedRole = static_cast<DataTypes::ColumnsRoles>(role);

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mAllTrackData[index.internalId()][DataTypes::TitleRole];
        break;
    case DataTypes::ElementTypeRole:
    case DataTypes::IdRole:
    case DataTypes::ParentIdRole:
    case DataTypes::TitleRole:
    case DataTypes::DurationRole:
    case DataTypes::ArtistRole:
    case DataTypes::AlbumRole:
        result = d->mAllTrackData[index.internalId()][convertedRole];
        break;
    case DataTypes::RatingRole:
        result = 0;
        break;
    case DataTypes::ResourceRole:
        result = QUrl{};
        break;
    case DataTypes::ImageUrlRole:
        switch (d->mAllTrackData[index.internalId()][DataTypes::ElementTypeRole].value<ElisaUtils::PlayListEntryType>())
        {
        case ElisaUtils::Album:
            if (!d->mAllTrackData[index.internalId()][DataTypes::ImageUrlRole].toString().isEmpty()) {
                result = d->mAllTrackData[index.internalId()][DataTypes::ImageUrlRole].toUrl();
            } else {
                if (d->mUseLocalIcons) {
                    result = QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
                } else {
                    result = QUrl(QStringLiteral("image://icon/media-optical-audio"));
                }
            }
            break;
        case ElisaUtils::Container:
        case ElisaUtils::UpnpMediaServer:
            if (!d->mAllTrackData[index.internalId()][DataTypes::ImageUrlRole].toString().isEmpty()) {
                result = d->mAllTrackData[index.internalId()][DataTypes::ImageUrlRole].toUrl();
            } else {
                if (d->mUseLocalIcons) {
                    return QUrl(QStringLiteral("qrc:/folder.svg"));
                } else {
                    return QUrl(QStringLiteral("image://icon/folder"));
                }
            }
            break;
        case ElisaUtils::Track:
            result = d->mAllTrackData[index.internalId()][DataTypes::ImageUrlRole];
            break;
        case ElisaUtils::Artist:
        case ElisaUtils::Composer:
        case ElisaUtils::FileName:
        case ElisaUtils::Genre:
        case ElisaUtils::Lyricist:
        case ElisaUtils::Radio:
        case ElisaUtils::Unknown:
            break;
        }
        break;
    case DataTypes::HasModelChildrenRole:
        result = rowCount(index);
        break;
    case DataTypes::FullDataRole:
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::data" << d->mAllTrackData[index.internalId()];
        result = QVariant::fromValue(static_cast<DataTypes::MusicDataType>(d->mAllTrackData[index.internalId()]));
        break;
    }

    return result;
}

QModelIndex UpnpContentDirectoryModel::index(int row, int column, const QModelIndex &parent) const
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::index" << row << column << parent;

    QModelIndex result;

    auto currentInternalId = parent.internalId();

    if (!parent.isValid()) {
        currentInternalId = d->mUpnpIds[parentId()];
    }

    if (!d->mChilds.contains(currentInternalId)) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::index" << row << column << parent << result;

        return result;
    }

    if (row < 0 || row >= d->mChilds[currentInternalId].size()) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::index" << row << column << parent << result;

        return result;
    }

    if (column != 0) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::index" << row << column << parent << result;

        return result;
    }

    result = createIndex(row, column, d->mChilds[currentInternalId][row]);

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::index" << row << column << parent << result;

    return result;
}

QModelIndex UpnpContentDirectoryModel::parent(const QModelIndex &child) const
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::parent" << child;

    QModelIndex result;

    // child is valid
    if (!child.isValid()) {
        return result;
    }

    // data knows child internal id
    if (!d->mAllTrackData.contains(child.internalId())) {
        return result;
    }

    const auto &childData = d->mAllTrackData[child.internalId()];

    // child data has upnp id of parent
    if (!childData.contains(DataTypes::ParentIdRole)) {
        return result;
    }

    const auto &parentStringId = childData[DataTypes::ParentIdRole].toString();

    // upnp ids has the internal id of parent
    if (!d->mUpnpIds.contains(parentStringId)) {
        return result;
    }

    // special case if we are already at top of model
    if (parentStringId == QLatin1Char('0')) {
        return result;
    }

    auto parentInternalId = d->mUpnpIds[parentStringId];

    // data knows parent internal id
    if (!d->mAllTrackData.contains(parentInternalId)) {
        return result;
    }

    const auto &parentData = d->mAllTrackData[parentInternalId];

    // parent data has upnp id of parent
    if (!parentData.contains(DataTypes::ParentIdRole)) {
        return result;
    }

    const auto &grandParentStringId = parentData[DataTypes::ParentIdRole].toString();

    // upnp ids has the internal id of grand parent
    if (!d->mUpnpIds.contains(grandParentStringId)) {
        return result;
    }

    auto grandParentInternalId = d->mUpnpIds[grandParentStringId];

    // childs of grand parent are known
    if (!d->mChilds.contains(grandParentInternalId)) {
        return result;
    }

    const auto &allUncles = d->mChilds[grandParentInternalId];

    // look for my parent
    for (int i = 0; i < allUncles.size(); ++i) {
        if (allUncles[i] == parentInternalId) {
            result = createIndex(i, 0, parentInternalId);
            break;
        }
    }

    return result;
}

int UpnpContentDirectoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 1;
}

bool UpnpContentDirectoryModel::canFetchMore(const QModelIndex &parent) const
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::canFetchMore" << parent;

    bool result = false;

    auto parentInternalId = parent.internalId();

    if (!parent.isValid()) {
        parentInternalId = d->mUpnpIds[parentId()];
    }

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::canFetchMore" << parent
                               << parentInternalId << !d->mChilds.contains(parentInternalId)
                               <<  d->mChilds[parentInternalId].isEmpty();

    result = !d->mChilds.contains(parentInternalId) ||  d->mChilds[parentInternalId].isEmpty();

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::canFetchMore" << parent << result;

    return result;
}

void UpnpContentDirectoryModel::fetchMore(const QModelIndex &parent)
{
    if (d->mIsBusy) {
        return;
    }

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::fetchMore" << parent;

    d->mIsBusy = true;
    Q_EMIT isBusyChanged();

    if (!d->mContentDirectory) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::fetchMore" << parent << "no content directory";

        d->mIsBusy = false;
        Q_EMIT isBusyChanged();

        return;
    }

    auto parentInternalId = parent.internalId();

    if (!parent.isValid()) {
        parentInternalId = d->mUpnpIds[parentId()];
    }

    if (!d->mAllTrackData.contains(parentInternalId)) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::fetchMore" << parent << "no parent internal id";

        d->mIsBusy = false;
        Q_EMIT isBusyChanged();

        return;
    }

    if (!d->mAllTrackData[parentInternalId].contains(DataTypes::IdRole)) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::fetchMore" << parent << "no id role";

        d->mIsBusy = false;
        Q_EMIT isBusyChanged();

        return;
    }

    if (parentId().isEmpty()) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::fetchMore" << parent << d->mAllTrackData[parentInternalId][DataTypes::IdRole].toString();

        d->mDidlParser.setParentId(d->mAllTrackData[parentInternalId][DataTypes::IdRole].toString());
    } else {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::fetchMore" << parent << parentId();

        d->mDidlParser.setParentId(parentId());
    }
    d->mDidlParser.browse();
}

const QString &UpnpContentDirectoryModel::parentId() const
{
    return d->mParentId;
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
    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::setContentDirectory" << directory;
    if (directory) {
        beginResetModel();
    }
    if (d->mContentDirectory) {
    }

    d->mContentDirectory = directory;

    if (!d->mContentDirectory) {
        Q_EMIT contentDirectoryChanged();
        return;
    }

    d->mDidlParser.setContentDirectory(d->mContentDirectory);
    d->mDidlParser.setBrowseFlag(browseFlag());
    d->mDidlParser.setFilter(filter());
    d->mDidlParser.setSortCriteria(sortCriteria());
    d->mDidlParser.setParentId(parentId());

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

bool UpnpContentDirectoryModel::isBusy() const
{
    return d->mIsBusy;
}

void UpnpContentDirectoryModel::initializeByData(MusicListenersManager *manager, DatabaseInterface *database,
                                                 ElisaUtils::PlayListEntryType modelType, ElisaUtils::FilterType filter,
                                                 const DataTypes::DataType &dataFilter)
{
    Q_UNUSED(database)
    Q_UNUSED(modelType)
    Q_UNUSED(filter)

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::initializeByData" << modelType << filter << dataFilter << dataFilter[DataTypes::UUIDRole].toString() << dataFilter[DataTypes::IdRole].toString();

    if (manager && manager->ssdpEngine() && manager->upnpServiceDiscovery()) {
        auto *newContentDirectory = new UpnpControlContentDirectory;

        const auto &deviceDescription = manager->upnpServiceDiscovery()->deviceDescriptionByUdn(dataFilter[DataTypes::UUIDRole].toString());
        newContentDirectory->setDescription(deviceDescription.serviceById(QStringLiteral("urn:upnp-org:serviceId:ContentDirectory")));
        setParentId(dataFilter[DataTypes::IdRole].toString());
        setContentDirectory(newContentDirectory);
        d->mDidlParser.setDeviceUUID(dataFilter[DataTypes::UUIDRole].toString());
    }
}

void UpnpContentDirectoryModel::setParentId(QString parentId)
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::setParentId" << parentId;

    if (d->mParentId == parentId) {
        return;
    }

    if (parentId.isEmpty()) {
        parentId = QStringLiteral("0");
    }

    d->mParentId = std::move(parentId);
    d->mDidlParser.setParentId(d->mParentId);

    Q_EMIT parentIdChanged();

    beginResetModel();
    d->mLastInternalId = 1;

    d->mUpnpIds[d->mParentId] = d->mLastInternalId;

    d->mChilds[d->mLastInternalId] = QVector<quintptr>();

    d->mAllTrackData[d->mLastInternalId] = {{DataTypes::IdRole, d->mParentId},
                                            {DataTypes::ElementTypeRole, QVariant::fromValue(ElisaUtils::Container)},
                                            {DataTypes::TitleRole, QStringLiteral("Root")},};

    ++d->mLastInternalId;

    d->mCurrentUpdateId = -1;
    endResetModel();
}

QModelIndex UpnpContentDirectoryModel::indexFromInternalId(quintptr internalId) const
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::indexFromInternalId" << internalId;

    QModelIndex result;

    if (internalId == 1) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::indexFromInternalId" << internalId << result;

        return result;
    }

    // data knows child internal id
    if (!d->mAllTrackData.contains(internalId)) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::indexFromInternalId" << internalId << "unknown children id" << result;

        return result;
    }

    const auto &childData = d->mAllTrackData[internalId];

    // child data has upnp id of parent
    if (!childData.contains(DataTypes::ParentIdRole)) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::indexFromInternalId" << internalId << "unknown parent" << result;

        return result;
    }

    const auto &parentStringId = childData[DataTypes::ParentIdRole].toString();

    // upnp ids has the internal id of parent
    if (!d->mUpnpIds.contains(parentStringId)) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::indexFromInternalId" << internalId << "unknown parent from ids list" << result;

        return result;
    }

    auto parentInternalId = d->mUpnpIds[parentStringId];

    // childs of parent are known
    if (!d->mChilds.contains(parentInternalId)) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::indexFromInternalId" << internalId << "unknown parent id" << result;

        return result;
    }

    const auto &allUncles = d->mChilds[parentInternalId];

    // look for my parent
    for (int i = 0; i < allUncles.size(); ++i) {
        if (allUncles[i] == internalId) {
            result = createIndex(i, 0, internalId);
            break;
        }
    }

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::indexFromInternalId" << internalId << result;

    return result;
}

void UpnpContentDirectoryModel::contentChanged(const QString &parentId)
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::contentChanged" << parentId;

    auto parentInternalId = d->mUpnpIds[parentId];

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::contentChanged" << parentId
                               << parentInternalId
                               << indexFromInternalId(parentInternalId)
                               << 0 << d->mDidlParser.newMusicTracks().size() - 1;
    beginInsertRows(indexFromInternalId(parentInternalId), 0, d->mDidlParser.newMusicTracks().size() - 1);

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::contentChanged" << parentId << parentInternalId;

    for(const auto &oneUpnpTrack : qAsConst(d->mDidlParser.newMusicTracks())) {
        d->mAllTrackData[d->mLastInternalId] = oneUpnpTrack;
        d->mUpnpIds[oneUpnpTrack[DataTypes::IdRole].toString()] = d->mLastInternalId;
        d->mChilds[parentInternalId].push_back(d->mLastInternalId);
        ++d->mLastInternalId;
    }

    qCDebug(orgKdeElisaUpnp()) << "UpnpContentDirectoryModel::contentChanged" << parentId << d->mChilds[parentInternalId].size();

    endInsertRows();

    d->mIsBusy = false;
    Q_EMIT isBusyChanged();
}

#include "moc_upnpcontentdirectorymodel.cpp"
