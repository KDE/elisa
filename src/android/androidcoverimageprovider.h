/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ANDROIDCOVERIMAGEPROVIDER_H
#define ANDROIDCOVERIMAGEPROVIDER_H

#include "elisaLib_export.h"

#include <QQuickAsyncImageProvider>
#include <QThreadPool>

class ELISALIB_EXPORT AndroidCoverImageProvider : public QQuickAsyncImageProvider
{
public:

    AndroidCoverImageProvider();

    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:

    QThreadPool pool;

};

#endif // ANDROIDCOVERIMAGEPROVIDER_H
