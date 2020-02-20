/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef LOCALFILELISTING_H
#define LOCALFILELISTING_H

#include "elisaLib_export.h"

#include "../abstractfile/abstractfilelisting.h"


#include <memory>

class LocalFileListingPrivate;

class ELISALIB_EXPORT LocalFileListing : public AbstractFileListing
{

    Q_OBJECT

public:

    explicit LocalFileListing(QObject *parent = nullptr);

    ~LocalFileListing() override;

private:

    void executeInit(QHash<QUrl, QDateTime> allFiles) override;

    void triggerRefreshOfContent() override;

    void triggerStop() override;

    DataTypes::TrackDataType scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo) override;

    std::unique_ptr<LocalFileListingPrivate> d;

};



#endif // LOCALFILELISTING_H
