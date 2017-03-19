/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef LOCALFILELISTING_H
#define LOCALFILELISTING_H

#include "../abstractfile/abstractfilelisting.h"

#include <QString>

#include <memory>

class LocalFileListingPrivate;

class LocalFileListing : public AbstractFileListing
{

    Q_OBJECT

    Q_PROPERTY(QString rootPath
               READ rootPath
               WRITE setRootPath
               NOTIFY rootPathChanged)

public:

    explicit LocalFileListing(QObject *parent = 0);

    virtual ~LocalFileListing();

    QString rootPath() const;

Q_SIGNALS:

    void rootPathChanged();

public Q_SLOTS:

    void setRootPath(QString rootPath);

private:

    void triggerRefreshOfContent() override;

    std::unique_ptr<LocalFileListingPrivate> d;

};



#endif // LOCALFILELISTING_H
