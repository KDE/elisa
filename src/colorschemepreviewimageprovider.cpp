/*
   SPDX-FileCopyrightText: 2022 (c) Tranter Madi <trmdi@yandex.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "colorschemepreviewimageprovider.h"

#include <KColorSchemeManager>

#include <QIcon>
#include <QModelIndex>

#if KCOLORSCHEME_VERSION < QT_VERSION_CHECK(6, 6, 0)
#include <memory>
#endif

ColorSchemePreviewImageProvider::ColorSchemePreviewImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ColorSchemePreviewImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
#if KCOLORSCHEME_VERSION < QT_VERSION_CHECK(6, 6, 0)
    auto schemes = std::make_unique<KColorSchemeManager>();
#else
    auto *schemes = KColorSchemeManager::instance();
#endif

    QModelIndex index = schemes->indexForScheme(id);

    // the id of the default entry must be set to an empty string
    if (!index.isValid()) {
        index = schemes->indexForScheme(QLatin1String(""));
    }
    const auto pixmap = schemes->model()->data(index, Qt::DecorationRole).value<QIcon>().pixmap(requestedSize);
    *size = pixmap.size();
    return pixmap;
}
