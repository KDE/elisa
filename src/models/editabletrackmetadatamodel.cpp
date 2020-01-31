/*
 * Copyright 2020 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "editabletrackmetadatamodel.h"

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
    mIsDirty = false;
    Q_EMIT isDirtyChanged();

    QString imageUrl = dataFromType(DataTypes::ImageUrlRole).toString();
    if (!imageUrl.isEmpty()
            && !imageUrl.startsWith(QStringLiteral("http://"))
            && !imageUrl.startsWith(QStringLiteral("https://"))
            && !imageUrl.startsWith(QStringLiteral("file://"))) {
        auto newTrackData = allTrackData();
        newTrackData[DataTypes::ImageUrlRole] = QStringLiteral("file:/").append(imageUrl);
        Q_EMIT saveRadioData(newTrackData);
    } else {
        Q_EMIT saveRadioData(allTrackData());
    }
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

    if (newValidState) {
        const auto &titleData = dataFromType(TrackDataType::key_type::TitleRole);

        newValidState = newValidState && !titleData.toString().isEmpty();
    }

    if (mIsDataValid != newValidState) {
        mIsDataValid = newValidState;

        Q_EMIT isDataValidChanged();
    }
}


#include "moc_editabletrackmetadatamodel.cpp"
