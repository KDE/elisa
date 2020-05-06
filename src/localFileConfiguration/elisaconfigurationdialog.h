/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#if !defined ELISACONFIGURATIONDIALOG_H_
#define ELISACONFIGURATIONDIALOG_H_

#include "elisaLib_export.h"

#include <QStringList>
#include <QFileSystemWatcher>

class ELISALIB_EXPORT ElisaConfigurationDialog : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QStringList rootPath
               READ rootPath
               WRITE setRootPath
               NOTIFY rootPathChanged)

    Q_PROPERTY(bool forceUsageOfFastFileSearch
               READ forceUsageOfFastFileSearch
               WRITE setForceUsageOfFastFileSearch
               NOTIFY forceUsageOfFastFileSearchChanged)

    Q_PROPERTY(bool showProgressInTaskBar
               READ showProgressInTaskBar
               WRITE setShowProgressInTaskBar
               NOTIFY showProgressInTaskBarChanged)

    Q_PROPERTY(bool showSystemTrayIcon
               READ showSystemTrayIcon
               WRITE setShowSystemTrayIcon
               NOTIFY showSystemTrayIconChanged)

    Q_PROPERTY(bool isDirty
               READ isDirty
               NOTIFY isDirtyChanged)

public:

    explicit ElisaConfigurationDialog(QObject *parent = nullptr);

    ~ElisaConfigurationDialog() override;

    QStringList rootPath() const;

    bool isDirty() const
    {
        return mIsDirty;
    }

    bool showProgressInTaskBar() const
    {
        return mShowProgressInTaskBar;
    }

    bool showSystemTrayIcon() const
    {
        return mShowSystemTrayIcon;
    }

    bool forceUsageOfFastFileSearch() const
    {
        return mForceUsageOfFastFileSearch;
    }

Q_SIGNALS:

    void rootPathChanged(const QStringList &rootPath);

    void isDirtyChanged();

    void showProgressInTaskBarChanged();

    void showSystemTrayIconChanged();

    void forceUsageOfFastFileSearchChanged();

public Q_SLOTS:

    void setRootPath(const QStringList &rootPath);

    void save();

    void setShowProgressInTaskBar(bool showProgressInTaskBar);

    void setShowSystemTrayIcon(bool showSystemTrayIcon);

    void setForceUsageOfFastFileSearch(bool forceUsageOfFastFileSearch);

private Q_SLOTS:

    void configChanged();

private:

    void setDirty();

    QStringList mRootPath;

    QFileSystemWatcher mConfigFileWatcher;

    bool mIsDirty = false;

    bool mShowProgressInTaskBar = true;

    bool mShowSystemTrayIcon = false;

    bool mForceUsageOfFastFileSearch = true;
};

#endif
