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

#ifndef EDITABLETRACKMETADATAMODEL_H
#define EDITABLETRACKMETADATAMODEL_H

#include "elisaLib_export.h"

#include "trackmetadatamodel.h"

class ELISALIB_EXPORT EditableTrackMetadataModel : public TrackMetadataModel
{
    Q_OBJECT

    Q_PROPERTY(bool isDataValid
               READ isDataValid
               NOTIFY isDataValidChanged)

    Q_PROPERTY(QString errorMessage
               READ errorMessage
               NOTIFY errorMessageChanged)

    Q_PROPERTY(bool isDirty
               READ isDirty
               NOTIFY isDirtyChanged)

public:
    explicit EditableTrackMetadataModel(QObject *parent = nullptr);

    bool isDataValid() const
    {
        return mIsDataValid;
    }

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    bool isDirty() const
    {
        return mIsDirty;
    }

    QString errorMessage() const
    {
        return mErrorMessage;
    }

Q_SIGNALS:
    void isDataValidChanged();

    void isDirtyChanged();

    void errorMessageChanged();

public Q_SLOTS:

    void saveData();

protected:

    void filterDataFromTrackData() override;

    void fillLyricsDataFromTrack() override;

private:

    void validData();

    bool mIsDataValid = false;

    bool mIsDirty = false;

    QString mErrorMessage;
};



#endif // EDITABLETRACKMETADATAMODEL_H
