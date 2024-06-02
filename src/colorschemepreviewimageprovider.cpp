/*
   SPDX-FileCopyrightText: 2022 (c) Tranter Madi <trmdi@yandex.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "colorschemepreviewimageprovider.h"

#include <KColorSchemeManager>

#include <QIcon>
#include <QModelIndex>

ColorSchemePreviewImageProvider::ColorSchemePreviewImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ColorSchemePreviewImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    KColorSchemeManager schemes;

    QModelIndex index = schemes.indexForScheme(id);

    // the id of the default entry must be set to an empty string
    if (!index.isValid()) {
        index = schemes.indexForScheme(QLatin1String(""));
    }
    const auto pixmap = schemes.model()->data(index, Qt::DecorationRole).value<QIcon>().pixmap(requestedSize);
    *size = pixmap.size();
    return pixmap;
}
