/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MEDIAPLAYLIST_H
#define MEDIAPLAYLIST_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "datatypes.h"

#include <QAbstractListModel>
#include <QMediaPlayer>

#include <utility>
#include <tuple>
#include <memory>

class MediaPlayListPrivate;
class MediaPlayListEntry;
class QDebug;

class ELISALIB_EXPORT MediaPlayList : public QAbstractListModel
{
    Q_OBJECT

public:

    enum ColumnsRoles {
        TitleRole = DataTypes::TitleRole,
        SecondaryTextRole,
        ImageUrlRole,
        ShadowForImageRole,
        ChildModelRole,
        DurationRole,
        StringDurationRole,
        ArtistRole,
        AllArtistsRole,
        HighestTrackRating,
        AlbumRole,
        AlbumArtistRole,
        IsValidAlbumArtistRole,
        TrackNumberRole,
        DiscNumberRole,
        RatingRole,
        GenreRole,
        LyricistRole,
        ComposerRole,
        CommentRole,
        YearRole,
        ChannelsRole,
        BitRateRole,
        SampleRateRole,
        ResourceRole,
        IdRole,
        ParentIdRole,
        DatabaseIdRole,
        IsSingleDiscAlbumRole,
        ContainerDataRole,
        IsPartialDataRole,
        AlbumIdRole,
        HasEmbeddedCover,
        FileModificationTime,
        FirstPlayDate,
        LastPlayDate,
        PlayCounter,
        PlayFrequency,
        ElementTypeRole,
        LyricsRole,
        FullDataRole,
        IsDirectoryRole,
        IsPlayListRole,
        FilePathRole,
        IsValidRole,
        CountRole,
        IsPlayingRole,
        AlbumSectionRole,
        MetadataModifiableRole,
    };

    Q_ENUM(ColumnsRoles)

    enum PlayState {
        NotPlaying,
        IsPlaying,
        IsPaused,
    };

    Q_ENUM(PlayState)

    using ListTrackDataType = DataTypes::ListTrackDataType;

    using TrackDataType = DataTypes::TrackDataType;

    explicit MediaPlayList(QObject *parent = nullptr);

    ~MediaPlayList() override;

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    void clearPlayList();

    void enqueueRestoredEntries(const QVariantList &newEntries);

    [[nodiscard]] QVariantList getEntriesForRestore() const;

Q_SIGNALS:

    void newTrackByNameInList(const QVariant &title, const QVariant &artist, const QVariant &album, const QVariant &trackNumber, const QVariant &discNumber);

    void newEntryInList(qulonglong newDatabaseId,
                        const QString &entryTitle,
                        ElisaUtils::PlayListEntryType databaseIdType);

    void newUrlInList(const QUrl &entryUrl,
                      ElisaUtils::PlayListEntryType databaseIdType);

public Q_SLOTS:

    void tracksListAdded(qulonglong newDatabaseId,
                         const QString &entryTitle,
                         ElisaUtils::PlayListEntryType databaseIdType,
                         const MediaPlayList::ListTrackDataType &tracks);

    void trackChanged(const MediaPlayList::TrackDataType &track);

    void trackRemoved(qulonglong trackId);

    void trackInError(const QUrl &sourceInError, QMediaPlayer::Error playerError);

    void enqueueFilesList(const DataTypes::EntryDataList &newEntries);

    void enqueueOneEntry(const DataTypes::EntryData &entryData);

    void enqueueMultipleEntries(const DataTypes::EntryDataList &entriesData);

private:

    std::unique_ptr<MediaPlayListPrivate> d;
};

class MediaPlayListEntry
{
public:

    MediaPlayListEntry() = default;

    explicit MediaPlayListEntry(qulonglong id) : mId(id), mIsValid(true) {
    }

    MediaPlayListEntry(QString title, QString artist,
                       QString album, int trackNumber,
                       int discNumber, ElisaUtils::PlayListEntryType entryType = ElisaUtils::Unknown)
        : mTitle(std::move(title)), mAlbum(std::move(album)), mArtist(std::move(artist)),
          mTrackNumber(trackNumber), mDiscNumber(discNumber), mEntryType(entryType) {
    }

    MediaPlayListEntry(qulonglong id, QVariant title, QVariant artist,
                       QVariant album, QVariant trackUrl,
                       QVariant trackNumber, QVariant discNumber,
                       ElisaUtils::PlayListEntryType entryType = ElisaUtils::Unknown)
        : mTitle(std::move(title)), mAlbum(std::move(album)), mArtist(std::move(artist)), mTrackUrl(std::move(trackUrl)),
          mTrackNumber(std::move(trackNumber)), mDiscNumber(std::move(discNumber)), mId(id), mEntryType(entryType) {
    }

    explicit MediaPlayListEntry(const MediaPlayList::TrackDataType &track)
        : mTitle(track[DataTypes::TitleRole]),
          mAlbum(track[DataTypes::AlbumRole]),
          mTrackNumber(track[DataTypes::TrackNumberRole]),
          mDiscNumber(track[DataTypes::DiscNumberRole]),
          mId(track[DataTypes::DatabaseIdRole].toULongLong()),
          mIsValid(true) {
    }

    explicit MediaPlayListEntry(QString artist) : mArtist(std::move(artist)), mEntryType(ElisaUtils::Artist) {
    }

    explicit MediaPlayListEntry(QUrl fileName) : mTrackUrl(std::move(fileName)) {
    }

    explicit MediaPlayListEntry(qulonglong id,
                                const QString &entryTitle,
                                ElisaUtils::PlayListEntryType type)
        : mTitle(entryTitle), mId(id), mIsValid(type == ElisaUtils::Track), mEntryType(type) {
    }

    QVariant mTitle;

    QVariant mAlbum;

    QVariant mArtist;

    QVariant mTrackUrl;

    QVariant mTrackNumber;

    QVariant mDiscNumber;

    qulonglong mId = 0;

    bool mIsValid = false;

    ElisaUtils::PlayListEntryType mEntryType = ElisaUtils::PlayListEntryType::Unknown;

    MediaPlayList::PlayState mIsPlaying = MediaPlayList::NotPlaying;

};

QDebug operator<<(const QDebug &stream, const MediaPlayListEntry &data);



#endif // MEDIAPLAYLIST_H
