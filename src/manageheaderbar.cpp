/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "manageheaderbar.h"

#include <QTime>

ManageHeaderBar::ManageHeaderBar(QObject *parent)
    : QObject(parent)
{
}

void ManageHeaderBar::setArtistRole(int value)
{
    mArtistRole = value;
    Q_EMIT artistRoleChanged();
}

int ManageHeaderBar::artistRole() const
{
    return mArtistRole;
}

void ManageHeaderBar::setTitleRole(int value)
{
    mTitleRole = value;
    Q_EMIT titleRoleChanged();
}

int ManageHeaderBar::titleRole() const
{
    return mTitleRole;
}

void ManageHeaderBar::setAlbumRole(int value)
{
    mAlbumRole = value;
    Q_EMIT albumRoleChanged();
}

void ManageHeaderBar::setAlbumArtistRole(int value)
{
    mAlbumArtistRole = value;
    Q_EMIT albumArtistRoleChanged();
}

void ManageHeaderBar::setFileNameRole(int value)
{
    mFileNameRole = value;
    Q_EMIT fileNameRoleChanged();
}

int ManageHeaderBar::albumRole() const
{
    return mAlbumRole;
}

int ManageHeaderBar::albumArtistRole() const
{
    return mAlbumArtistRole;
}

int ManageHeaderBar::fileNameRole() const
{
    return mFileNameRole;
}

void ManageHeaderBar::setImageRole(int value)
{
    mImageRole = value;
    Q_EMIT imageRoleChanged();
}

void ManageHeaderBar::setDatabaseIdRole(int databaseIdRole)
{
    mDatabaseIdRole = databaseIdRole;
    Q_EMIT databaseIdRoleChanged();
}

void ManageHeaderBar::setTrackTypeRole(int trackTypeRole)
{
    mTrackTypeIdRole = trackTypeRole;
    Q_EMIT trackTypeRoleChanged();
}

int ManageHeaderBar::imageRole() const
{
    return mImageRole;
}

int ManageHeaderBar::databaseIdRole() const
{
    return mDatabaseIdRole;
}

int ManageHeaderBar::trackTypeRole() const
{
    return mTrackTypeIdRole;
}

void ManageHeaderBar::setAlbumIdRole(int albumIdRole)
{
    mAlbumIdRole = albumIdRole;
    Q_EMIT albumIdRoleChanged();
}

int ManageHeaderBar::albumIdRole() const
{
    return mAlbumIdRole;
}

QVariant ManageHeaderBar::album() const
{
    QVariant result = QString{};

    if (!mCurrentTrack.isValid() || mCurrentTrack.data(mAlbumRole).isNull()) {
        return result;
    }

    result = mCurrentTrack.data(mAlbumRole);

    return result;
}

QVariant ManageHeaderBar::albumArtist() const
{
    if (!mCurrentTrack.isValid()) {
        return QString();
    }

    return mCurrentTrack.data(mAlbumArtistRole);
}

QUrl ManageHeaderBar::fileUrl() const
{
    QUrl result;

    if (!mCurrentTrack.isValid()) {
        return result;
    }

    result = mCurrentTrack.data(mFileNameRole).toUrl();

    return result;
}

QVariant ManageHeaderBar::title() const
{
    if (!mCurrentTrack.isValid()) {
        return QString();
    }

    return mCurrentTrack.data(mTitleRole);
}

QVariant ManageHeaderBar::artist() const
{
    if (!mCurrentTrack.isValid()) {
        return QString();
    }

    auto artistValue = mCurrentTrack.data(mArtistRole);

    if (!artistValue.isValid()) {
        return mCurrentTrack.data(mAlbumArtistRole);
    }

    return artistValue;
}

QUrl ManageHeaderBar::image() const
{
    if (!mCurrentTrack.isValid()) {
        return {};
    }

    return mCurrentTrack.data(mImageRole).toUrl();
}

qulonglong ManageHeaderBar::databaseId() const
{
    if (!mCurrentTrack.isValid()) {
        return 0;
    }

    return mCurrentTrack.data(mDatabaseIdRole).toULongLong();
}

ElisaUtils::PlayListEntryType ManageHeaderBar::trackType() const
{
    if (!mCurrentTrack.isValid()) {
        return ElisaUtils::Unknown;
    }

    return mCurrentTrack.data(mTrackTypeIdRole).value<ElisaUtils::PlayListEntryType>();
}

qulonglong ManageHeaderBar::albumId() const
{
    if (!mCurrentTrack.isValid()) {
        return 0;
    }

    return mCurrentTrack.data(mAlbumIdRole).toULongLong();
}

bool ManageHeaderBar::isValid() const
{
    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return mCurrentTrack.data(mIsValidRole).toBool();
}

int ManageHeaderBar::isValidRole() const
{
    return mIsValidRole;
}

QPersistentModelIndex ManageHeaderBar::currentTrack() const
{
    return mCurrentTrack;
}

void ManageHeaderBar::notifyArtistProperty()
{
    auto newArtistValue = mCurrentTrack.data(mArtistRole);
    if (mOldArtist != newArtistValue) {
        Q_EMIT artistChanged();

        mOldArtist = newArtistValue;
    }
}

void ManageHeaderBar::notifyTitleProperty()
{
    auto newTitleValue = mCurrentTrack.data(mTitleRole);
    if (mOldTitle != newTitleValue) {
        Q_EMIT titleChanged();

        mOldTitle = newTitleValue;
    }
}

void ManageHeaderBar::notifyAlbumProperty()
{
    auto newAlbumValue = mCurrentTrack.data(mAlbumRole);
    if (mOldAlbum != newAlbumValue) {
        Q_EMIT albumChanged();

        mOldAlbum = newAlbumValue;
    }
}

void ManageHeaderBar::notifyAlbumArtistProperty()
{
    auto newAlbumArtistValue = mCurrentTrack.data(mAlbumArtistRole);
    if (mOldAlbumArtist != newAlbumArtistValue) {
        Q_EMIT albumArtistChanged();

        mOldAlbumArtist = newAlbumArtistValue;
    }
}

void ManageHeaderBar::notifyFileNameProperty()
{
    auto newFileNameValue = mCurrentTrack.data(mFileNameRole);
    if (mOldFileName != newFileNameValue) {
        Q_EMIT fileUrlChanged();

        mOldFileName = newFileNameValue;
    }
}

void ManageHeaderBar::notifyImageProperty()
{
    auto newImageValue = mCurrentTrack.data(mImageRole);
    if (mOldImage != newImageValue) {
        Q_EMIT imageChanged();

        mOldImage = newImageValue;
    }
}

void ManageHeaderBar::notifyDatabaseIdProperty()
{
    bool conversionOk;
    auto newDatabaseIdValue = mCurrentTrack.data(mDatabaseIdRole).toULongLong(&conversionOk);
    if (conversionOk && mOldDatabaseId != newDatabaseIdValue) {
        Q_EMIT databaseIdChanged();

        mOldDatabaseId = newDatabaseIdValue;
    } else if (!conversionOk && mOldDatabaseId != 0) {
        Q_EMIT databaseIdChanged();

        mOldDatabaseId = 0;
    }
}

void ManageHeaderBar::notifyTrackTypeProperty()
{
    auto newTrackTypeValue = mCurrentTrack.data(mTrackTypeIdRole).value<ElisaUtils::PlayListEntryType>();
    if (mOldTrackType != newTrackTypeValue) {
        Q_EMIT trackTypeChanged();

        mOldTrackType = newTrackTypeValue;
    }
}

void ManageHeaderBar::notifyAlbumIdProperty()
{
    bool conversionOk;
    auto newAlbumIdValue = mCurrentTrack.data(mAlbumIdRole).toULongLong(&conversionOk);
    if (conversionOk && mOldAlbumId != newAlbumIdValue) {
        Q_EMIT albumIdChanged();

        mOldAlbumId = newAlbumIdValue;
    } else if (!conversionOk && mOldAlbumId != 0) {
        Q_EMIT albumIdChanged();

        mOldAlbumId = 0;
    }
}

void ManageHeaderBar::notifyIsValidProperty()
{
    auto newIsValidValue = mCurrentTrack.data(mIsValidRole).toBool();
    if (mOldIsValid != newIsValidValue) {
        Q_EMIT isValidChanged();

        mOldIsValid = newIsValidValue;
    }
}

void ManageHeaderBar::setIsValidRole(int isValidRole)
{
    mIsValidRole = isValidRole;
    Q_EMIT isValidRoleChanged();
}

void ManageHeaderBar::setCurrentTrack(const QPersistentModelIndex &currentTrack)
{
    mCurrentTrack = currentTrack;
    Q_EMIT currentTrackChanged();
    updateCurrentTrackData();
}

void ManageHeaderBar::updateCurrentTrackData()
{
    notifyArtistProperty();
    notifyTitleProperty();
    notifyAlbumProperty();
    notifyAlbumArtistProperty();
    notifyFileNameProperty();
    notifyImageProperty();
    notifyDatabaseIdProperty();
    notifyTrackTypeProperty();
    notifyAlbumIdProperty();
    notifyIsValidProperty();
}

#include "moc_manageheaderbar.cpp"
