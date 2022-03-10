/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

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
    return mSchemes->model()->data(index, Qt::DecorationRole).value<QIcon>().pixmap(requestedSize);
}
