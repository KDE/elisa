/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef GENERICDATAMODEL_H
#define GENERICDATAMODEL_H

#include "elisautils.h"

#include <QAbstractListModel>

#include <memory>

class GenericDataModelPrivate;
class ModelDataCache;

class GenericDataModel : public QAbstractListModel
{

    Q_OBJECT

    Q_PROPERTY(ElisaUtils::DataType dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)

    Q_PROPERTY(ModelDataCache* modelCache READ modelCache WRITE setModelCache NOTIFY modelCacheChanged)

    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:

    explicit GenericDataModel(QObject *parent = nullptr);

    ~GenericDataModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    ElisaUtils::DataType dataType() const;

    ModelDataCache *modelCache() const;

    bool isBusy() const;

Q_SIGNALS:

    void neededData(int row) const;

    void dataTypeChanged(ElisaUtils::DataType dataType);

    void modelCacheChanged(ModelDataCache* modelCache);

    void isBusyChanged(bool isBusy);

public Q_SLOTS:

    void receiveData(int row);

    void setDataType(ElisaUtils::DataType dataType);

    void setModelCache(ModelDataCache* modelCache);

    void modelDataChanged(int lowerBound, int upperBound);

private:

    void resetModelType();

    std::unique_ptr<GenericDataModelPrivate> d;

};



#endif // GENERICDATAMODEL_H
