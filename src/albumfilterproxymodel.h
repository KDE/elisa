/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef ALBUMFILTERPROXYMODEL_H
#define ALBUMFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QRegularExpression>

class AlbumFilterProxyModel : public QSortFilterProxyModel
{

    Q_OBJECT

    Q_PROPERTY(QString filterText
               READ filterText
               WRITE setFilterText
               NOTIFY filterTextChanged)

public:

    explicit AlbumFilterProxyModel(QObject *parent = 0);

    virtual ~AlbumFilterProxyModel();

    QString filterText() const;

public Q_SLOTS:

    void setFilterText(QString filterText);

Q_SIGNALS:

    void filterTextChanged(QString filterText);

protected:

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:

    QString mFilterText;

    QRegularExpression mFilterExpression;

};

#endif // ALBUMFILTERPROXYMODEL_H
