/*
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef TRACKMETADATAPROXYMODEL_H
#define TRACKMETADATAPROXYMODEL_H

#include "elisaLib_export.h"

#include <QQmlEngine>
#include <QSortFilterProxyModel>

class ELISALIB_EXPORT TrackMetadataProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    QML_ELEMENT

    Q_PROPERTY(bool showTagsWithNoData READ showTagsWithNoData WRITE setShowTagsWithNoData NOTIFY showTagsWithNoDataChanged)

public:
    explicit TrackMetadataProxyModel(QObject *parent = nullptr);

    ~TrackMetadataProxyModel() override;

    [[nodiscard]] Q_INVOKABLE int mapRowToSource(const int proxyRow) const;

    [[nodiscard]] bool showTagsWithNoData() const;

Q_SIGNALS:

    void showTagsWithNoDataChanged();

public Q_SLOTS:

    void setShowTagsWithNoData(bool showTagsWithNoData);

protected:
    [[nodiscard]] bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    bool mShowTagsWithNoData = false;
};

#endif // TRACKMETADATAPROXYMODEL_H
