/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FILELISTENER_H
#define FILELISTENER_H

#include "../abstractfile/abstractfilelistener.h"

#include <QObject>

class FileListenerPrivate;
class LocalFileListing;

class FileListener : public AbstractFileListener
{
    Q_OBJECT

public:
    explicit FileListener(QObject *parent = nullptr);

    ~FileListener() override;

    [[nodiscard]] const LocalFileListing& localFileIndexer() const;

private:

    std::unique_ptr<FileListenerPrivate> d;

};

#endif // FILELISTENER_H
