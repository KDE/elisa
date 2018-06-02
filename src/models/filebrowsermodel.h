/*
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
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

#ifndef FILEBROWSERMODEL_H
#define FILEBROWSERMODEL_H

#include <KIOWidgets/KDirModel>
#include <KFileMetaData/ExtractorCollection>
#include <QMimeDatabase>

class MusicAudioTrack;

class FileBrowserModel : public KDirModel
{
    Q_OBJECT

public:

    enum ColumnsRoles {
        NameRole = Qt::UserRole + 1,
        ContainerDataRole = Qt::UserRole + 2,
        ImageUrlRole = Qt::UserRole + 3,
        DirectoryRole = Qt::UserRole + 4
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
