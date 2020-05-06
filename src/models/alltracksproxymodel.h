/*
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ALLTRACKSPROXYMODEL_H
#define ALLTRACKSPROXYMODEL_H

#include "elisaLib_export.h"

#include "abstractmediaproxymodel.h"
#include "elisautils.h"

class ELISALIB_EXPORT AllTracksProxyModel : public AbstractMediaProxyModel
{
    Q_OBJECT

public:

    explicit AllTracksProxyModel(QObject *parent = nullptr);

    ~AllTracksProxyModel() override;

Q_SIGNALS:

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
