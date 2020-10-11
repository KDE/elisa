/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef VIEWSMODEL_H
#define VIEWSMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "datatypes.h"

#include <QAbstractListModel>

#include <memory>

class ViewsModelPrivate;
class ViewsListData;

class ELISALIB_EXPORT ViewsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(ViewsListData* viewsData READ viewsData WRITE setViewsData NOTIFY viewsDataChanged)

public:
    enum ColumnRoles {
        ImageNameRole = Qt::UserRole + 1,
        SecondTitleRole,
        UseSecondTitleRole,
        DatabaseIdRole,
        EntryTypeRole,
        UseColorOverlayRole,
        EntryCategoryRole,
    };

    explicit ViewsModel(QObject *parent = nullptr);

    ~ViewsModel() override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QModelIndex parent(const QModelIndex &child) const override;

    [[nodiscard]] ViewsListData* viewsData() const;

Q_SIGNALS:

    void viewsDataChanged();

public Q_SLOTS:

    void setViewsData(ViewsListData* viewsData);

private Q_SLOTS:

    void dataAboutToBeAdded(int startIndex, int lastIndex);

    void dataAdded();

    void dataAboutToBeRemoved(int startIndex, int lastIndex);

    void dataRemoved();

    void dataModified(int currentIndex);

    void dataAboutToBeReset();

    void dataReset();

private:

    std::unique_ptr<ViewsModelPrivate> d;
};

#endif // VIEWSMODEL_H
