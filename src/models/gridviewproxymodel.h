/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef GRIDVIEWPROXYMODEL_H
#define GRIDVIEWPROXYMODEL_H

#include "elisaLib_export.h"

#include "abstractmediaproxymodel.h"

#include <QQmlEngine>

class ELISALIB_EXPORT GridViewProxyModel : public AbstractMediaProxyModel
{

    Q_OBJECT

    Q_PROPERTY(int tracksCount
               READ tracksCount
               NOTIFY tracksCountChanged)

public:

    explicit GridViewProxyModel(QObject *parent = nullptr);

    ~GridViewProxyModel() override;

    [[nodiscard]] int tracksCount() const;

Q_SIGNALS:

    void tracksCountChanged();

public Q_SLOTS:

protected:

    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

};

#endif // GRIDVIEWPROXYMODEL_H
