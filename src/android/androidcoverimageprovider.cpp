/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.coom>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "androidcoverimageprovider.h"

#include "android/androidcommon.h"

#include <QImage>
#include <QJniEnvironment>
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>

#include <android/bitmap.h>
#include <jni.h>

QImage androidBitmapToImage(QJniObject qbitmap)
{
    QJniEnvironment qenv;
    AndroidBitmapInfo info;

    if (const int error = AndroidBitmap_getInfo(qenv.jniEnv(), qbitmap.object(), &info); error != ANDROID_BITMAP_RESULT_SUCCESS) {
        qCDebug(orgKdeElisaAndroid()) << "androidBitmapToImage error code" << error;
        return {};
    }

    if (info.width == 0 || info.height == 0) {
        return {};
    }

    QImage::Format format;
    switch (info.format) {
    case ANDROID_BITMAP_FORMAT_A_8:
        format = QImage::Format_Alpha8;
        break;
    case ANDROID_BITMAP_FORMAT_RGBA_1010102:
        format = QImage::Format_A2BGR30_Premultiplied;
        break;
    case ANDROID_BITMAP_FORMAT_RGBA_4444:
        format = QImage::Format_ARGB4444_Premultiplied;
        break;
    case ANDROID_BITMAP_FORMAT_RGBA_8888:
        format = QImage::Format_RGBA8888;
        break;
    case ANDROID_BITMAP_FORMAT_RGBA_F16:
        format = QImage::Format_RGBA16FPx4;
        break;
    case ANDROID_BITMAP_FORMAT_RGB_565:
        format = QImage::Format_RGB16;
        break;
    case ANDROID_BITMAP_FORMAT_NONE:
    default:
        return {};
    }

    void *pixels;
    if (AndroidBitmap_lockPixels(qenv.jniEnv(), qbitmap.object(), &pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
        return {};
    }

    QImage image{reinterpret_cast<uchar *>(pixels), static_cast<int>(info.width), static_cast<int>(info.height), format};

    if (AndroidBitmap_unlockPixels(qenv.jniEnv(), qbitmap.object()) != ANDROID_BITMAP_RESULT_SUCCESS) {
        return {};
    }

    return image;
}

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
        mErrorMessage = QLatin1String{""};

#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
        QJniObject bitmap = QJniObject::callStaticObjectMethod("org/kde/elisa/ElisaActivity", "contentThumbnail",
                                                               "(Landroid/content/Context;Ljava/lang/String;II)Landroid/graphics/Bitmap;",
                                                               QNativeInterface::QAndroidApplication::context(), QJniObject::fromString(mId).object<jstring>(),
                                                               mRequestedSize.width(), mRequestedSize.height());
#else
        QJniObject bitmap = QJniObject::callStaticObjectMethod("org/kde/elisa/ElisaActivity", "contentThumbnail",
                                                               "(Landroid/content/Context;Ljava/lang/String;II)Landroid/graphics/Bitmap;",
                                                               QNativeInterface::QAndroidApplication::context().object<jobject>(), QJniObject::fromString(mId).object<jstring>(),
                                                               mRequestedSize.width(), mRequestedSize.height());
#endif
        mCoverImage = androidBitmapToImage(bitmap);

        if (mCoverImage.isNull()) {
            qCDebug(orgKdeElisaAndroid()) << "AsyncImageResponse::run failed for" << mId;
            mErrorMessage = QString{QLatin1String{"Invalid android cover image in "} + mId};
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

AndroidCoverImageProvider::AndroidCoverImageProvider()
    : QQuickAsyncImageProvider()
{
}

QQuickImageResponse *AndroidCoverImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    auto response = std::make_unique<AsyncImageResponse>(id, requestedSize);
    pool.start(response.get());
    return response.release();
}

#include "androidcoverimageprovider.moc"
