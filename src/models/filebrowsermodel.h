/*
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FILEBROWSERMODEL_H
#define FILEBROWSERMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "datatypes.h"

#include <KIOWidgets/KDirModel>

class MusicListenersManager;
class DatabaseInterface;

class ELISALIB_EXPORT FileBrowserModel : public KDirModel
{
    Q_OBJECT

    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:

    explicit FileBrowserModel(QObject *parent = nullptr);

    ~FileBrowserModel() override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setUrl(const QUrl &url);

    [[nodiscard]] bool isBusy() const;

Q_SIGNALS:

    void isBusyChanged();

public Q_SLOTS:

    void initialize(MusicListenersManager *manager, DatabaseInterface *database,
                    ElisaUtils::PlayListEntryType modelType, ElisaUtils::FilterType filter,
                    const QString &genre, const QString &artist, qulonglong databaseId,
                    const QUrl &pathFilter);

    void initializeByData(MusicListenersManager *manager, DatabaseInterface *database,
                          ElisaUtils::PlayListEntryType modelType, ElisaUtils::FilterType filter,
                          const DataTypes::DataType &dataFilter);
};



#endif //FILEBROWSERMODEL_H
