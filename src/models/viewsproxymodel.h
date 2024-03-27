/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef VIEWSPROXYMODEL_H
#define VIEWSPROXYMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"

#include <QQmlEngine>
#include <QSortFilterProxyModel>

#include <memory>

class ViewsProxyModelPrivate;

class ELISALIB_EXPORT ViewsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    QML_ELEMENT

    Q_PROPERTY(ElisaUtils::PlayListEntryType embeddedCategory
               READ embeddedCategory
               WRITE setEmbeddedCategory
               NOTIFY embeddedCategoryChanged)

public:
    explicit ViewsProxyModel(QObject *parent = nullptr);

    ~ViewsProxyModel() override;

    [[nodiscard]] ElisaUtils::PlayListEntryType embeddedCategory() const;

    Q_INVOKABLE int mapRowToSource(int row) const;

    Q_INVOKABLE int mapRowFromSource(int row) const;

Q_SIGNALS:
    void embeddedCategoryChanged();

public Q_SLOTS:
    void setEmbeddedCategory(const ElisaUtils::PlayListEntryType category);

protected:

    [[nodiscard]] bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    std::unique_ptr<ViewsProxyModelPrivate> d;
};

#endif // VIEWSPROXYMODEL_H
