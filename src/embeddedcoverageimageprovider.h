/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef EMBEDDEDCOVERAGEIMAGEPROVIDER_H
#define EMBEDDEDCOVERAGEIMAGEPROVIDER_H

#include <QQuickAsyncImageProvider>
#include <QThreadPool>

class EmbeddedCoverageImageProvider : public QQuickAsyncImageProvider
{
public:

    EmbeddedCoverageImageProvider();

    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:

    QThreadPool pool;

};

#endif // EMBEDDEDCOVERAGEIMAGEPROVIDER_H
