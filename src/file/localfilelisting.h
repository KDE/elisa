/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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

    DataTypes::TrackDataType scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo, FileSystemWatchingModes watchForFileSystemChanges) override;

    std::unique_ptr<LocalFileListingPrivate> d;

};



#endif // LOCALFILELISTING_H
