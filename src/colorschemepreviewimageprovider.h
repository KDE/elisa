/*
   SPDX-FileCopyrightText: 2022 (c) Tranter Madi <trmdi@yandex.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef COLORSCHEMEPREVIEWIMAGEPROVIDER_H
#define COLORSCHEMEPREVIEWIMAGEPROVIDER_H

#include "elisaLib_export.h"

#include <QQuickImageProvider>

class KColorSchemeManager;

class ELISALIB_EXPORT ColorSchemePreviewImageProvider : public QQuickImageProvider
{
public:

    ColorSchemePreviewImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

};

#endif // COLORSCHEMEPREVIEWIMAGEPROVIDER_H
