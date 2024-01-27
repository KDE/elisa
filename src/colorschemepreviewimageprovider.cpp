/*
   SPDX-FileCopyrightText: 2022 (c) Tranter Madi <trmdi@yandex.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "colorschemepreviewimageprovider.h"

#include <KColorSchemeManager>

#include <QIcon>
#include <QModelIndex>

ColorSchemePreviewImageProvider::ColorSchemePreviewImageProvider(KColorSchemeManager *schemes)
    : QQuickImageProvider(QQuickImageProvider::Pixmap), mSchemes(schemes)
{
}

QPixmap ColorSchemePreviewImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (!mSchemes) {
        return QPixmap();
    }
    QModelIndex index = mSchemes->indexForScheme(id);

    // the id of the default entry must be set to an empty string
    if (!index.isValid()) {
        index = mSchemes->indexForScheme(QStringLiteral(""));
    }
    const auto pixmap = mSchemes->model()->data(index, Qt::DecorationRole).value<QIcon>().pixmap(requestedSize);
    *size = pixmap.size();
    return pixmap;
}
