/*
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

#ifndef VIEWSMODEL_H
#define VIEWSMODEL_H

#include "elisaLib_export.h"

#include "viewmanager.h"

#include <QAbstractListModel>

#include <memory>

class ViewsModelPrivate;

class ELISALIB_EXPORT ViewsModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum ColumnRoles {
        ItemType = Qt::UserRole + 1,
        ImageName,
    };

    explicit ViewsModel(QObject *parent = nullptr);

    ~ViewsModel() override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    Q_INVOKABLE int indexFromViewType(ViewManager::ViewsType type);

private:

    std::unique_ptr<ViewsModelPrivate> d;

};

#endif // VIEWSMODEL_H
