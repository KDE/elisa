/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#if !defined ELISACONFIGURATIONDIALOG_H_
#define ELISACONFIGURATIONDIALOG_H_

#include "elisaLib_export.h"

#include "elisautils.h"

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

    Q_PROPERTY(ElisaUtils::PlayListEntryType embeddedView
               READ embeddedView
               WRITE setEmbeddedView
               NOTIFY embeddedViewChanged)

    Q_PROPERTY(bool isDirty
               READ isDirty
               NOTIFY isDirtyChanged)

    Q_PROPERTY(bool playAtStartup
               READ playAtStartup
               WRITE setPlayAtStartup
               NOTIFY playAtStartupChanged)

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

    ElisaUtils::PlayListEntryType embeddedView() const
    {
        return mEmbeddedView;
    }

    bool playAtStartup() const
    {
        return mPlayAtStartup;
    }

Q_SIGNALS:

    void rootPathChanged(const QStringList &rootPath);

    void isDirtyChanged();

    void showProgressInTaskBarChanged();

    void showSystemTrayIconChanged();

    void forceUsageOfFastFileSearchChanged();

    void embeddedViewChanged();

    void playAtStartupChanged();

public Q_SLOTS:

    void setRootPath(const QStringList &rootPath);

    void save();

    void setShowProgressInTaskBar(bool showProgressInTaskBar);

    void setShowSystemTrayIcon(bool showSystemTrayIcon);

    void setForceUsageOfFastFileSearch(bool forceUsageOfFastFileSearch);

    void setEmbeddedView(ElisaUtils::PlayListEntryType embeddedView);

    void setPlayAtStartup(bool playAtStartup);

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

    bool mPlayAtStartup = false;

    ElisaUtils::PlayListEntryType mEmbeddedView = ElisaUtils::Unknown;
};

#endif
