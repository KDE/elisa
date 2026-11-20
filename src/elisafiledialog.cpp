/*
    SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

    SPDX-License-Identifier: LGPL-3.0-or-later
*/

#include "elisafiledialog.h"
#include "playerLogging.h"

#include <QFileDialog>

ElisaFileDialog::ElisaFileDialog(QObject *parent)
    : QObject(parent)
{
}

ElisaFileDialog::~ElisaFileDialog() = default;

ElisaFileDialog::FileMode ElisaFileDialog::fileMode() const
{
    return mFileMode;
}

void ElisaFileDialog::setFileMode(FileMode mode)
{
    if (mFileMode == mode) {
        return;
    }
    mFileMode = mode;
    Q_EMIT fileModeChanged();
}

QStringList ElisaFileDialog::nameFilters() const
{
    return mNameFilters;
}

void ElisaFileDialog::setNameFilters(const QStringList &filters)
{
    if (mNameFilters == filters) {
        return;
    }
    mNameFilters = filters;
    Q_EMIT nameFiltersChanged();
}

QString ElisaFileDialog::defaultSuffix() const
{
    return mDefaultSuffix;
}

void ElisaFileDialog::setDefaultSuffix(const QString &suffix)
{
    if (mDefaultSuffix == suffix) {
        return;
    }
    mDefaultSuffix = suffix;
    Q_EMIT defaultSuffixChanged();
}

QUrl ElisaFileDialog::currentFolder() const
{
    return mCurrentFolder;
}

void ElisaFileDialog::setCurrentFolder(const QUrl &folder)
{
    if (mCurrentFolder == folder) {
        return;
    }
    mCurrentFolder = folder;
    Q_EMIT currentFolderChanged();
}

QUrl ElisaFileDialog::selectedFile() const
{
    return mSelectedFiles.value(0);
}

QList<QUrl> ElisaFileDialog::selectedFiles() const
{
    return mSelectedFiles;
}

QWindow *ElisaFileDialog::transientParent() const
{
    return mTransientParent;
}

void ElisaFileDialog::setTransientParent(QWindow *parent)
{
    if (mTransientParent == parent) {
        return;
    }
    mTransientParent = parent;
    Q_EMIT transientParentChanged();
}

void ElisaFileDialog::open()
{
    auto *dialog = new QFileDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    switch (mFileMode) {
    case OpenFile:
        dialog->setAcceptMode(QFileDialog::AcceptOpen);
        dialog->setFileMode(QFileDialog::ExistingFile);
        break;
    case OpenFiles:
        dialog->setAcceptMode(QFileDialog::AcceptOpen);
        dialog->setFileMode(QFileDialog::ExistingFiles);
        break;
    case SaveFile:
        dialog->setAcceptMode(QFileDialog::AcceptSave);
        dialog->setFileMode(QFileDialog::AnyFile);
        if (!mDefaultSuffix.isEmpty()) {
            dialog->setDefaultSuffix(mDefaultSuffix);
        }
        break;
    }

    if (!mNameFilters.isEmpty()) {
        dialog->setNameFilters(mNameFilters);
    }

    if (mCurrentFolder.isValid()) {
        dialog->setDirectoryUrl(mCurrentFolder);
    }

    if (mTransientParent) {
        dialog->setWindowModality(Qt::WindowModal);
        dialog->winId();
        if (dialog->windowHandle()) {
            dialog->windowHandle()->setTransientParent(mTransientParent);
        }
    } else {
        dialog->setWindowModality(Qt::ApplicationModal);
    }

    connect(dialog, &QFileDialog::accepted, this, [this, dialog]() {
        const QList<QUrl> urls = dialog->selectedUrls();
        qCDebug(orgKdeElisaPlayer()) << "ElisaFileDialog accepted" << "mode:" << mFileMode << "selectedUrls:" << urls << "directoryUrl:" << dialog->directoryUrl();
        if (mSelectedFiles != urls) {
            const bool firstChanged = selectedFile() != urls.value(0);
            mSelectedFiles = urls;
            if (firstChanged) {
                Q_EMIT selectedFileChanged();
            }
            Q_EMIT selectedFilesChanged();
        }
        if (dialog->directoryUrl().isValid() && dialog->directoryUrl() != mCurrentFolder) {
            mCurrentFolder = dialog->directoryUrl();
            Q_EMIT currentFolderChanged();
        }
        Q_EMIT accepted();
    });

    connect(dialog, &QFileDialog::rejected, this, &ElisaFileDialog::rejected);

    dialog->open();
}

#include "moc_elisafiledialog.cpp"
