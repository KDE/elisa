/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
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

#ifndef GRIDVIEWPROXYMODEL_H
#define GRIDVIEWPROXYMODEL_H

#include "elisaLib_export.h"

#include "abstractmediaproxymodel.h"
#include "elisautils.h"

class ELISALIB_EXPORT GridViewProxyModel : public AbstractMediaProxyModel
{

    Q_OBJECT

    Q_PROPERTY(ElisaUtils::PlayListEntryType dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)

public:

    explicit GridViewProxyModel(QObject *parent = nullptr);

    ~GridViewProxyModel() override;

    ElisaUtils::PlayListEntryType dataType() const;

Q_SIGNALS:

    void entriesToEnqueue(const ElisaUtils::EntryDataList &newEntries,
                          ElisaUtils::PlayListEntryType databaseIdType,
                          ElisaUtils::PlayListEnqueueMode enqueueMode,
                          ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void dataTypeChanged();

public Q_SLOTS:

    void enqueueToPlayList();

    void replaceAndPlayOfPlayList();

    void setDataType(ElisaUtils::PlayListEntryType newDataType);

protected:

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:

    void genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode,
                                  ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    ElisaUtils::PlayListEntryType mDataType = ElisaUtils::Unknown;

};

#endif // GRIDVIEWPROXYMODEL_H
