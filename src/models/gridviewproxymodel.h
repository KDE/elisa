/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
