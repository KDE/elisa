/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
