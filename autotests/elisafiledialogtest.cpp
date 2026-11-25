/*
    SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

    SPDX-License-Identifier: LGPL-3.0-or-later
*/

#include "elisafiledialog.h"

#include <QApplication>
#include <QFileDialog>
#include <QSignalSpy>
#include <QTest>
#include <QUrl>
#include <QWindow>

class ElisaFileDialogTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testPropertiesAndSignals()
    {
        ElisaFileDialog dialog;

        QCOMPARE(dialog.fileMode(), ElisaFileDialog::OpenFile);
        QCOMPARE(dialog.isOpened(), false);

        QSignalSpy fileModeSpy(&dialog, &ElisaFileDialog::fileModeChanged);
        dialog.setFileMode(ElisaFileDialog::SaveFile);
        QCOMPARE(dialog.fileMode(), ElisaFileDialog::SaveFile);
        QCOMPARE(fileModeSpy.count(), 1);
        dialog.setFileMode(ElisaFileDialog::SaveFile);
        QCOMPARE(fileModeSpy.count(), 1);

        QSignalSpy nameFiltersSpy(&dialog, &ElisaFileDialog::nameFiltersChanged);
        const QStringList filters{QStringLiteral("Audio (*.mp3 *.flac)")};
        dialog.setNameFilters(filters);
        QCOMPARE(dialog.nameFilters(), filters);
        QCOMPARE(nameFiltersSpy.count(), 1);
        dialog.setNameFilters(filters);
        QCOMPARE(nameFiltersSpy.count(), 1);

        QSignalSpy defaultSuffixSpy(&dialog, &ElisaFileDialog::defaultSuffixChanged);
        dialog.setDefaultSuffix(QStringLiteral("m3u8"));
        QCOMPARE(dialog.defaultSuffix(), QStringLiteral("m3u8"));
        QCOMPARE(defaultSuffixSpy.count(), 1);

        QSignalSpy currentFolderSpy(&dialog, &ElisaFileDialog::currentFolderChanged);
        const QUrl folder = QUrl::fromLocalFile(QStringLiteral("/tmp"));
        dialog.setCurrentFolder(folder);
        QCOMPARE(dialog.currentFolder(), folder);
        QCOMPARE(currentFolderSpy.count(), 1);

        QWindow parentWindow;
        QSignalSpy transientParentSpy(&dialog, &ElisaFileDialog::transientParentChanged);
        dialog.setTransientParent(&parentWindow);
        QCOMPARE(dialog.transientParent(), &parentWindow);
        QCOMPARE(transientParentSpy.count(), 1);
    }

    void testOpenGuardAndDestructionCleanup()
    {
        auto *dialog = new ElisaFileDialog();
        QSignalSpy openedSpy(dialog, &ElisaFileDialog::openedChanged);
        QSignalSpy rejectedSpy(dialog, &ElisaFileDialog::rejected);

        dialog->open();
        QCOMPARE(dialog->isOpened(), true);
        QCOMPARE(openedSpy.count(), 1);

        // Calling open() again while already open must not spawn a second
        // QFileDialog
        dialog->open();
        QCOMPARE(openedSpy.count(), 1);
        int openFileDialogs = 0;
        for (QWidget *w : QApplication::topLevelWidgets()) {
            if (qobject_cast<QFileDialog *>(w)) {
                ++openFileDialogs;
            }
        }
        QCOMPARE(openFileDialogs, 1);

        // Destroying ElisaFileDialog while open must close and clean up the
        // underlying QFileDialog
        delete dialog;
        QCOMPARE(rejectedSpy.count(), 0);
        QCoreApplication::processEvents();
        int remainingFileDialogs = 0;
        for (QWidget *w : QApplication::topLevelWidgets()) {
            if (qobject_cast<QFileDialog *>(w)) {
                ++remainingFileDialogs;
            }
        }
        QCOMPARE(remainingFileDialogs, 0);
    }
};

QTEST_MAIN(ElisaFileDialogTest)

#include "elisafiledialogtest.moc"
