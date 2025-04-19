/*
   SPDX-FileCopyrightText: 2025 (c) Pedro Nishiyama <nishiyama.v3@gmail.com>

   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "androidplayerjni.h"

AndroidPlayerJni::AndroidPlayerJni(QObject *parent)
    : QObject(parent)
{
}

AndroidPlayerJni::~AndroidPlayerJni() = default;

void AndroidPlayerJni::next(JNIEnv *, jobject)
{
    Q_EMIT AndroidPlayerJni::GetInstance().Next();
}

void AndroidPlayerJni::previous(JNIEnv *, jobject)
{
    Q_EMIT AndroidPlayerJni::GetInstance().Previous();
}

void AndroidPlayerJni::playPause(JNIEnv *, jobject)
{
    Q_EMIT AndroidPlayerJni::GetInstance().PlayPause();
}

void AndroidPlayerJni::stop(JNIEnv *, jobject)
{
    Q_EMIT AndroidPlayerJni::GetInstance().Stop();
}

void AndroidPlayerJni::seek(JNIEnv *, jobject, qlonglong Offset)
{
    Q_EMIT AndroidPlayerJni::GetInstance().Seek(Offset);
}

QJniObject AndroidPlayerJni::metadataToJObject(const QVariantMap &metadata)
{
    QJniEnvironment env;
    jclass clazz = env.findClass("org/kde/elisa/Metadata");

    QJniObject jMetadata = QJniObject(clazz,
                                      QJniObject::fromString(metadata[QStringLiteral("title")].toString()).object<jstring>(),
                                      QJniObject::fromString(metadata[QStringLiteral("artist")].toString()).object<jstring>(),
                                      QJniObject::fromString(metadata[QStringLiteral("albumName")].toString()).object<jstring>(),
                                      QJniObject::fromString(metadata[QStringLiteral("albumCover")].toString()).object<jstring>(),
                                      (jlong)metadata[QStringLiteral("duration")].toLongLong());

    return jMetadata;
}

void AndroidPlayerJni::propertyChanged(const QString &property, const QVariant &value)
{
    QJniObject jValue;

    switch (value.userType()) {
    case QMetaType::Int:
        jValue = QJniObject("java/lang/Integer", (jint)value.toInt());
        break;
    case QMetaType::LongLong:
        jValue = QJniObject("java/lang/Long", (jlong)value.toLongLong());
        break;
    case QMetaType::QString:
        jValue = QJniObject::fromString(value.toString());
        break;
    case QMetaType::QVariantMap:
        jValue = metadataToJObject(value.toMap());
        break;
    }

    if (!jValue.isValid()) {
        return;
    }

    QJniObject::callStaticMethod<void>("org/kde/elisa/Player",
                                       "propertyChanged",
                                       "(Ljava/lang/String;Ljava/lang/Object;)V",
                                       QJniObject::fromString(property).object<jstring>(),
                                       jValue.object<jobject>());
}

void AndroidPlayerJni::registerNativeMethods()
{
    const JNINativeMethod methods[] = {
        {"next", "()V", reinterpret_cast<void *>(AndroidPlayerJni::next)},
        {"previous", "()V", reinterpret_cast<void *>(AndroidPlayerJni::previous)},
        {"playPause", "()V", reinterpret_cast<void *>(AndroidPlayerJni::playPause)},
        {"stop", "()V", reinterpret_cast<void *>(AndroidPlayerJni::stop)},
        {"seek", "(J)V", reinterpret_cast<void *>(AndroidPlayerJni::seek)},
    };

    QJniEnvironment env;
    jclass clazz = env.findClass("org/kde/elisa/Player");
    env.registerNativeMethods(clazz, methods, sizeof(methods) / sizeof(methods[0]));
}

#include "moc_androidplayerjni.cpp"
