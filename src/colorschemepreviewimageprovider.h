/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

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
