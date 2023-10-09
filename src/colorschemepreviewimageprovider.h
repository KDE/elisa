/*
   SPDX-FileCopyrightText: 2022 (c) Tranter Madi <trmdi@yandex.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef COLORSCHEMEPREVIEWIMAGEPROVIDER_H
#define COLORSCHEMEPREVIEWIMAGEPROVIDER_H

#include <QQuickImageProvider>

class KColorSchemeManager;

class ColorSchemePreviewImageProvider : public QQuickImageProvider
{
public:

    ColorSchemePreviewImageProvider(KColorSchemeManager *schemes);
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

private:

    KColorSchemeManager *mSchemes;
};

#endif // COLORSCHEMEPREVIEWIMAGEPROVIDER_H
