/*
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "filebrowsermodel.h"
#include "datatypes.h"

#include <QUrl>
#include <QString>
#include <QMimeDatabase>
#include <KIOWidgets/KDirLister>

#include "models/modelLogging.h"

FileBrowserModel::FileBrowserModel(QObject *parent) : KDirModel(parent)
{
    QMimeDatabase db;
    const QList<QMimeType> mimeList = db.allMimeTypes();
    QStringList mimeTypes = { QStringLiteral("inode/directory") };
    for (const QMimeType &mime : mimeList) {
        if (mime.name().startsWith(QLatin1String("audio/"))) {
            mimeTypes << mime.name();
        }
    }

    dirLister()->setMimeFilter(mimeTypes);
}

FileBrowserModel::~FileBrowserModel()
= default;

QString FileBrowserModel::url() const
{
    return dirLister()->url().toString();
}

void FileBrowserModel::setUrl(const QString &url)
{
    QString path = QUrl(url).path();
    path = QUrl::fromLocalFile(path).toString();

    if (dirLister()->url().path() == QUrl(path).path()) {
        dirLister()->updateDirectory(QUrl(path));
        return;
    }

    beginResetModel();
    dirLister()->openUrl(QUrl(path));

    endResetModel();
    emit urlChanged();
}

bool FileBrowserModel::isBusy() const
{
    return false;
}

QHash<int, QByteArray> FileBrowserModel::roleNames() const
{
    auto roles = KDirModel::roleNames();

    roles[static_cast<int>(DataTypes::ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(DataTypes::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(DataTypes::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(DataTypes::ColumnsRoles::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(DataTypes::ColumnsRoles::ElementTypeRole)] = "dataType";
    roles[static_cast<int>(DataTypes::ColumnsRoles::ResourceRole)] = "url";

    roles[static_cast<int>(DataTypes::ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(DataTypes::ColumnsRoles::AllArtistsRole)] = "allArtists";
    roles[static_cast<int>(DataTypes::ColumnsRoles::HighestTrackRating)] = "highestTrackRating";
    roles[static_cast<int>(DataTypes::ColumnsRoles::GenreRole)] = "genre";

    roles[static_cast<int>(DataTypes::ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(DataTypes::ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(DataTypes::ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(DataTypes::ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(DataTypes::ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(DataTypes::ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(DataTypes::ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";
    roles[static_cast<int>(DataTypes::ColumnsRoles::FullDataRole)] = "fullData";

    roles[static_cast<int>(DataTypes::ColumnsRoles::IsDirectoryRole)] = "isDirectory";
    roles[static_cast<int>(DataTypes::ColumnsRoles::IsPlayListRole)] = "isPlaylist";

    return roles;
}

QVariant FileBrowserModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    qCDebug(orgKdeElisaModel()) << "FileBrowserModel::data" << index << role;

    switch(role)
    {
    case DataTypes::ColumnsRoles::ResourceRole:
    {
        KFileItem item = itemForIndex(index);
        result = item.url();
        break;
    }
    case DataTypes::ColumnsRoles::ImageUrlRole:
    {
        KFileItem item = itemForIndex(index);
        if (item.isDir()) {
            result = QUrl(QStringLiteral("image://icon/folder"));
        } else {
            result = QUrl(QStringLiteral("image://icon/audio-x-generic"));
        }
        break;
    }
    case DataTypes::ColumnsRoles::IsDirectoryRole:
    {
        KFileItem item = itemForIndex(index);
        result = item.isDir();
        break;
    }
    case DataTypes::ColumnsRoles::IsPlayListRole:
    {
        KFileItem item = itemForIndex(index);
        result = (item.currentMimeType().inherits(QStringLiteral("audio/x-mpegurl")));
        break;
    }
    case DataTypes::ColumnsRoles::ElementTypeRole:
    {
        KFileItem item = itemForIndex(index);
        result = (item.isDir() ? ElisaUtils::Container : ElisaUtils::FileName);
        break;
    }
    default:
        result = KDirModel::data(index,role);
    }

    qCDebug(orgKdeElisaModel()) << "FileBrowserModel::data" << index << role << result;

    return result;
}

void FileBrowserModel::initialize(MusicListenersManager *manager, DatabaseInterface *database,
                                  ElisaUtils::PlayListEntryType modelType, ElisaUtils::FilterType filter,
                                  const QString &genre, const QString &artist, qulonglong databaseId,
                                  const QUrl &pathFilter)
{
    Q_UNUSED(manager)
    Q_UNUSED(database)
    Q_UNUSED(modelType)
    Q_UNUSED(filter)
    Q_UNUSED(genre)
    Q_UNUSED(artist)
    Q_UNUSED(databaseId)

    setUrl(pathFilter.toLocalFile());
}


#include "moc_filebrowsermodel.cpp"
