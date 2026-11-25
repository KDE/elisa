/*
    SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

    SPDX-License-Identifier: LGPL-3.0-or-later
*/

#pragma once

#include "elisaLib_export.h"

#include <QList>
#include <QObject>
#include <QPointer>
#include <QQmlEngine>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QWindow>

class QFileDialog;

/**
 * QML-accessible wrapper around QFileDialog so non-local KIO URLs (e.g.
 * sftp://) are preserved without QtQuick.Dialogs::FileDialog's local-file-only
 * check.
 *
 * This is fixed in Qt 6.13. ElisaFileDialog should be replaced with
 * QtQuick.Dialogs.FileDialog when Qt 6.13 is the minimum requried version. Use
 * the fileMode FileDialog.OpenUrls.
 */
class ELISALIB_EXPORT ElisaFileDialog : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(FileMode fileMode READ fileMode WRITE setFileMode NOTIFY fileModeChanged)
    Q_PROPERTY(QStringList nameFilters READ nameFilters WRITE setNameFilters NOTIFY nameFiltersChanged)
    Q_PROPERTY(QString defaultSuffix READ defaultSuffix WRITE setDefaultSuffix NOTIFY defaultSuffixChanged)
    Q_PROPERTY(QUrl currentFolder READ currentFolder WRITE setCurrentFolder NOTIFY currentFolderChanged)
    Q_PROPERTY(QUrl selectedFile READ selectedFile NOTIFY selectedFileChanged)
    Q_PROPERTY(QList<QUrl> selectedFiles READ selectedFiles NOTIFY selectedFilesChanged)
    Q_PROPERTY(QWindow *transientParent READ transientParent WRITE setTransientParent NOTIFY transientParentChanged)
    Q_PROPERTY(bool opened READ isOpened NOTIFY openedChanged)

public:
    enum FileMode {
        OpenFile,
        OpenFiles,
        SaveFile,
    };
    Q_ENUM(FileMode)

    explicit ElisaFileDialog(QObject *parent = nullptr);
    ~ElisaFileDialog() override;

    [[nodiscard]] FileMode fileMode() const;
    void setFileMode(FileMode mode);

    [[nodiscard]] QStringList nameFilters() const;
    void setNameFilters(const QStringList &filters);

    [[nodiscard]] QString defaultSuffix() const;
    void setDefaultSuffix(const QString &suffix);

    [[nodiscard]] QUrl currentFolder() const;
    void setCurrentFolder(const QUrl &folder);

    [[nodiscard]] QUrl selectedFile() const;
    [[nodiscard]] QList<QUrl> selectedFiles() const;

    [[nodiscard]] QWindow *transientParent() const;
    void setTransientParent(QWindow *parent);

    [[nodiscard]] bool isOpened() const;

    Q_INVOKABLE void open();

Q_SIGNALS:
    void accepted();
    void rejected();
    void fileModeChanged();
    void nameFiltersChanged();
    void defaultSuffixChanged();
    void currentFolderChanged();
    void selectedFileChanged();
    void selectedFilesChanged();
    void transientParentChanged();
    void openedChanged();

private:
    FileMode mFileMode = OpenFile;
    QStringList mNameFilters;
    QString mDefaultSuffix;
    QUrl mCurrentFolder;
    QList<QUrl> mSelectedFiles;
    QPointer<QWindow> mTransientParent;
    QPointer<QFileDialog> mDialog;
};
