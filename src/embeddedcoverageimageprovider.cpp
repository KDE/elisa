/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "embeddedcoverageimageprovider.h"

#include <KFileMetaData/EmbeddedImageData>
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>

#include <QImage>
#include <QMimeDatabase>

namespace
{
class AsyncImageResponse : public QQuickImageResponse, public QRunnable
{
    Q_OBJECT

public:
    AsyncImageResponse(QString id, QSize requestedSize)
        : QQuickImageResponse(), mId(std::move(id)), mRequestedSize(requestedSize)
    {
        setAutoDelete(false);

        if (!mRequestedSize.width()) {
            mRequestedSize.setWidth(mRequestedSize.height());
        }

        if (!mRequestedSize.height()) {
            mRequestedSize.setHeight(mRequestedSize.width());
        }
    }

    [[nodiscard]] QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(mCoverImage);
    }

    void run() override
    {
        QMimeDatabase mimeDatabase;
        const auto fileMimeType = mimeDatabase.mimeTypeForFile(mId).name();
        KFileMetaData::ExtractorCollection ec;
        KFileMetaData::SimpleExtractionResult result(mId, fileMimeType, KFileMetaData::ExtractionResult::ExtractImageData);

        mErrorMessage = QLatin1String{""};

        const auto extractors = ec.fetchExtractors(fileMimeType);
        for (const auto& ex : extractors) {
            ex->extract(&result);
        }

        auto imageData = result.imageData();

        if (imageData.isEmpty()) {
          mErrorMessage = QString{QLatin1String{"Unable to load image data from "} + mId};
          Q_EMIT finished();
          return;
        }

        if (imageData.contains(KFileMetaData::EmbeddedImageData::FrontCover)) {
          mCoverImage = QImage::fromData(imageData[KFileMetaData::EmbeddedImageData::FrontCover]);
        } else {
          mCoverImage = QImage::fromData(imageData.first());
        }

        if (mCoverImage.isNull()) {
          mErrorMessage = QString{QLatin1String{"Invalid embedded cover image in "} + mId};
          Q_EMIT finished();
          return;
        }

        auto newCoverImage = mCoverImage.scaled(mRequestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if (!newCoverImage.isNull()) {
          mCoverImage = std::move(newCoverImage);
        }

        Q_EMIT finished();
    }

    QString errorString() const override
    {
      return mErrorMessage;
    }

    QString mId;
    QString mErrorMessage;
    QSize mRequestedSize;
    QImage mCoverImage;
};
}

EmbeddedCoverageImageProvider::EmbeddedCoverageImageProvider()
    : QQuickAsyncImageProvider()
{
}

QQuickImageResponse *EmbeddedCoverageImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    const QString decodedId = QUrl::fromPercentEncoding(id.toUtf8());
    auto response = std::make_unique<AsyncImageResponse>(decodedId, requestedSize);
    pool.start(response.get());
    return response.release();
}

#include "embeddedcoverageimageprovider.moc"
