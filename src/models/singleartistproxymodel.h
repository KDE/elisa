/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SINGLEARTISTPROXYMODEL_H
#define SINGLEARTISTPROXYMODEL_H

#include "abstractmediaproxymodel.h"

class SingleArtistProxyModel : public AbstractMediaProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QString artistFilter
               READ artistFilter
               WRITE setArtistFilterText
               NOTIFY artistFilterTextChanged)

public:
    QString artistFilter() const;

public Q_SLOTS:

    void enqueueToPlayList() override;

    void setArtistFilterText(const QString &filterText);

Q_SIGNALS:

    void artistFilterTextChanged(const QString &filterText);

protected:

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    QString mArtistFilter;

    QRegularExpression mArtistExpression;

};

#endif // SINGLEARTISTPROXYMODEL_H
