/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ANDROIDFILELISTING_H
#define ANDROIDFILELISTING_H

#include "../abstractfile/abstractfilelisting.h"

#include "datatypes.h"

#include <QObject>
#include <QString>
#include <QUrl>
#include <QHash>
#include <QVector>

#include <memory>

class AndroidFileListingPrivate;
class MusicAudioTrack;

class AndroidFileListing : public AbstractFileListing
{

    Q_OBJECT

public:
    AndroidFileListing(QObject *parent = nullptr);

    ~AndroidFileListing() override;

    void applicationAboutToQuit() override;

    static void registerNativeMethods();

    static AndroidFileListing* currentInstance();

    void androidMusicTracksScanStarted();

    void newMusicTrack(const QString &trackDescription);

    void androidMusicTracksScanFinished();

    void androidMusicAlbumsScanStarted();

    void newMusicAlbum(const QString &albumDescription);

    void androidMusicAlbumsScanFinished();

    void readExternalStoragePermissionIsOk();

    void readExternalStoragePermissionIsKo();

Q_SIGNALS:

public Q_SLOTS:

private Q_SLOTS:

private:
    void executeInit(QHash<QUrl, QDateTime> allFiles) override;

    void triggerRefreshOfContent() override;

    DataTypes::TrackDataType scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo, FileSystemWatchingModes watchForFileSystemChanges) override;

    static AndroidFileListing* mCurrentInstance;

    std::unique_ptr<AndroidFileListingPrivate> d;

};

#endif // ANDROIDFILELISTING_H
