/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "embeddedcoverageimageprovider.h"

#include <KFileMetaData/EmbeddedImageData>
#include <QImage>

class AsyncImageResponse : public QQuickImageResponse, public QRunnable
{
public:
    AsyncImageResponse(QString id, QSize requestedSize)
        : QQuickImageResponse(), mId(std::move(id)), mRequestedSize(requestedSize)
    {
        setAutoDelete(false);
    }

    QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(mCoverImage);
    }

    void run() override
    {
        KFileMetaData::EmbeddedImageData embeddedImage;

        auto imageData = embeddedImage.imageData(mId);

        if (imageData.contains(KFileMetaData::EmbeddedImageData::FrontCover)) {
            mCoverImage = QImage::fromData(imageData[KFileMetaData::EmbeddedImageData::FrontCover]);
        }

        emit finished();
    }

    QString mId;
    QSize mRequestedSize;
    QImage mCoverImage;
};

EmbeddedCoverageImageProvider::EmbeddedCoverageImageProvider()
    : QQuickAsyncImageProvider()
{
}

QQuickImageResponse *EmbeddedCoverageImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    auto response = std::make_unique<AsyncImageResponse>(id, requestedSize);
    pool.start(response.get());
    return response.release();
}
