/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef VIEWSPROXYMODEL_H
#define VIEWSPROXYMODEL_H

#include "elisaLib_export.h"

#include <QSortFilterProxyModel>

class ELISALIB_EXPORT ViewsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ViewsProxyModel(QObject *parent = nullptr);

protected:

    [[nodiscard]] bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};

#endif // VIEWSPROXYMODEL_H
