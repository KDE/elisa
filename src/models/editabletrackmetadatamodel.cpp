/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "editabletrackmetadatamodel.h"

#include <KI18n/KLocalizedString>

#include <QDebug>

EditableTrackMetadataModel::EditableTrackMetadataModel(QObject *parent)
    : TrackMetadataModel(parent)
{
}

bool EditableTrackMetadataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto result = TrackMetadataModel::setData(index, value, role);

    if (result) {
        if (!mIsDirty) {
            mIsDirty = true;
            Q_EMIT isDirtyChanged();
        }

        validData();
    }

    return result;
}

void EditableTrackMetadataModel::saveData()
{
    mIsNewRadio = false;
    mIsDirty = false;
    Q_EMIT isDirtyChanged();

    auto newTrackData = allTrackData();

    newTrackData[DataTypes::ResourceRole] = QUrl::fromUserInput(newTrackData[DataTypes::ResourceRole].toString());

    QString imageUrl = dataFromType(DataTypes::ImageUrlRole).toString();
    if (!imageUrl.isEmpty()
            && !imageUrl.startsWith(QStringLiteral("http://"))
            && !imageUrl.startsWith(QStringLiteral("https://"))
            && !imageUrl.startsWith(QStringLiteral("file://"))) {
        newTrackData[DataTypes::ImageUrlRole] = QUrl::fromUserInput(QStringLiteral("file:/").append(imageUrl));
    }

    Q_EMIT saveRadioData(newTrackData);
}

void EditableTrackMetadataModel::deleteRadio()
{
    auto &track = displayedTrackData();
    if (track[DataTypes::DatabaseIdRole].toULongLong()) {
        Q_EMIT deleteRadioData(track[DataTypes::DatabaseIdRole].toULongLong());
    }
}

void EditableTrackMetadataModel::fillDataFromTrackData(const TrackMetadataModel::TrackDataType &trackData,
                                                       const QList<DataTypes::ColumnsRoles> &fieldsForTrack)
{
    if (mIsNewRadio) {
        return;
    }

    TrackMetadataModel::fillDataFromTrackData(trackData, fieldsForTrack);
}

void EditableTrackMetadataModel::filterDataFromTrackData()
{
    TrackMetadataModel::filterDataFromTrackData();
    validData();
}

void EditableTrackMetadataModel::fillLyricsDataFromTrack()
{
    TrackMetadataModel::fillLyricsDataFromTrack();
    validData();
}

void EditableTrackMetadataModel::fillDataForNewRadio()
{
    mIsNewRadio = true;
    TrackMetadataModel::fillDataForNewRadio();
}

void EditableTrackMetadataModel::initialize(MusicListenersManager *newManager, DatabaseInterface *trackDatabase)
{
    TrackMetadataModel::initialize(newManager, trackDatabase);

    connect(this, &EditableTrackMetadataModel::saveRadioData,
            &modelDataLoader(), &ModelDataLoader::saveRadioModified);
    connect(this, &EditableTrackMetadataModel::deleteRadioData,
            &modelDataLoader(), &ModelDataLoader::removeRadio);
}

void EditableTrackMetadataModel::validData()
{
    bool newValidState = true;

    const auto &resourceData = dataFromType(TrackDataType::key_type::ResourceRole);
    if (resourceData.canConvert<QUrl>()) {
        const auto resourceUrl = resourceData.toUrl();

        newValidState = !resourceUrl.scheme().isEmpty() && resourceUrl.isValid() && !resourceUrl.isRelative();
    } else {
        newValidState = false;
    }

    if (!newValidState) {
        mErrorMessage = i18nc("Error message when track URL is invalid", "Invalid URL.");
        Q_EMIT errorMessageChanged();
    }

    if (newValidState) {
        const auto &titleData = dataFromType(TrackDataType::key_type::TitleRole);

        newValidState = newValidState && !titleData.toString().isEmpty();

        if (!newValidState) {
            mErrorMessage = i18nc("Error message when track title is empty", "Empty title.");
            Q_EMIT errorMessageChanged();
        }
    }

    if (mIsDataValid != newValidState) {
        mIsDataValid = newValidState;

        Q_EMIT isDataValidChanged();
    }
}


#include "moc_editabletrackmetadatamodel.cpp"
