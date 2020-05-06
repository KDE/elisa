/*
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FILEBROWSERMODEL_H
#define FILEBROWSERMODEL_H

#include "elisaLib_export.h"

#include <KIOWidgets/KDirModel>


class ELISALIB_EXPORT FileBrowserModel : public KDirModel
{
    Q_OBJECT

public:

    enum ColumnsRoles {
        NameRole = Qt::UserRole + 1,
        FileUrlRole = Qt::UserRole + 2,
        ImageUrlRole = Qt::UserRole + 3,
        IsDirectoryRole = Qt::UserRole + 4,
        IsPlayListRole = Qt::UserRole + 5
    };

    Q_ENUM(ColumnsRoles)

    explicit FileBrowserModel(QObject *parent = nullptr);

    ~FileBrowserModel() override;

    QString url() const;

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setUrl(const QString &url);

Q_SIGNALS:

    void urlChanged();

};

#endif //FILEBROWSERMODEL_H
