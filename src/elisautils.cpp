/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisautils.h"

#include <KFileMetaData/EmbeddedImageData>

QImage ElisaUtils::getEmbeddedImage(const QSize &size, const QString &identifier)
{
    QImage result;
    KFileMetaData::EmbeddedImageData embeddedImage;

    const auto &imageData = embeddedImage.imageData(identifier);

    for(const auto &oneCover : imageData) {
        if (!oneCover.isEmpty()) {
            result = QImage::fromData(oneCover);
            auto newCoverImage = result.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            if (!newCoverImage.isNull()) {
                result = std::move(newCoverImage);
            }
        }
    }

    return result;
}

QByteArray ElisaUtils::getEmbeddedImageData(const QString &identifier)
{
    QByteArray result;
    KFileMetaData::EmbeddedImageData embeddedImage;

    const auto &imageData = embeddedImage.imageData(identifier);

    for(const auto &oneCover : imageData) {
        if (!oneCover.isEmpty()) {
            result = std::move(oneCover);
        }
    }

    return result;
}

#include "moc_elisautils.cpp"
