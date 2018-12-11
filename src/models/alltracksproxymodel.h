/*
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef ALLTRACKSPROXYMODEL_H
#define ALLTRACKSPROXYMODEL_H

#include "elisaLib_export.h"

#include "abstractmediaproxymodel.h"
#include "musicaudiotrack.h"
#include "elisautils.h"

class ELISALIB_EXPORT AllTracksProxyModel : public AbstractMediaProxyModel
{
    Q_OBJECT

public:

    explicit AllTracksProxyModel(QObject *parent = nullptr);

    ~AllTracksProxyModel() override;

Q_SIGNALS:

    void trackToEnqueue(const ElisaUtils::EntryDataList &newEntries,
                        ElisaUtils::PlayListEntryType databaseIdType,
                        ElisaUtils::PlayListEnqueueMode enqueueMode,
                        ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

public Q_SLOTS:

    void enqueueToPlayList();

    void replaceAndPlayOfPlayList();

protected:

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:

    void genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode,
                                  ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

};

#endif // ALLTRACKSPROXYMODEL_H
