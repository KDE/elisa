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

    Q_PROPERTY(bool alwaysUseAbsolutePlaylistPaths
               READ alwaysUseAbsolutePlaylistPaths
               WRITE setAlwaysUseAbsolutePlaylistPaths
               NOTIFY alwaysUseAbsolutePlaylistPathsChanged)

    Q_PROPERTY(bool showNowPlayingBackground
               READ showNowPlayingBackground
               WRITE setShowNowPlayingBackground
               NOTIFY showNowPlayingBackgroundChanged)

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

    Q_PROPERTY(int initialViewIndex
               READ initialViewIndex
               WRITE setInitialViewIndex
               NOTIFY initialViewIndexChanged)

    Q_PROPERTY(QString initialFilesViewPath
               READ initialFilesViewPath
               WRITE setInitialFilesViewPath
               NOTIFY initialFilesViewPathChanged)

    Q_PROPERTY(bool isDirty
               READ isDirty
               NOTIFY isDirtyChanged)

    Q_PROPERTY(bool playAtStartup
               READ playAtStartup
               WRITE setPlayAtStartup
               NOTIFY playAtStartupChanged)

    Q_PROPERTY(bool scanAtStartup
               READ scanAtStartup
               WRITE setScanAtStartup
               NOTIFY scanAtStartupChanged)

    Q_PROPERTY(QString colorScheme
               READ colorScheme
               WRITE setColorScheme
               NOTIFY colorSchemeChanged)

    Q_PROPERTY(bool useFavoriteStyleRatings
               READ useFavoriteStyleRatings
               WRITE setUseFavoriteStyleRatings
               NOTIFY useFavoriteStyleRatingsChanged)

public:

    explicit ElisaConfigurationDialog(QObject *parent = nullptr);

    ~ElisaConfigurationDialog() override;

    [[nodiscard]] QStringList rootPath() const;

    [[nodiscard]] bool isDirty() const
    {
        return mIsDirty;
    }

    [[nodiscard]] bool showNowPlayingBackground() const
    {
        return mShowNowPlayingBackground;
    }


    [[nodiscard]] bool showProgressInTaskBar() const
    {
        return mShowProgressInTaskBar;
    }

    [[nodiscard]] bool showSystemTrayIcon() const
    {
        return mShowSystemTrayIcon;
    }

    [[nodiscard]] bool alwaysUseAbsolutePlaylistPaths() const
    {
        return mAlwaysUseAbsolutePlaylistPaths;
    }

    [[nodiscard]] ElisaUtils::PlayListEntryType embeddedView() const
    {
        return mEmbeddedView;
    }

    [[nodiscard]] int initialViewIndex() const
    {
        return mInitialViewIndex;
    }

    [[nodiscard]] QString initialFilesViewPath() const
    {
        return mInitialFilesViewPath;
    }

    [[nodiscard]] bool playAtStartup() const
    {
        return mPlayAtStartup;
    }

    [[nodiscard]] bool scanAtStartup() const
    {
        return mScanAtStartup;
    }

    [[nodiscard]] QString colorScheme() const
    {
        return mColorScheme;
    }

    [[nodiscard]] bool useFavoriteStyleRatings() const
    {
        return mUseFavoriteStyleRatings;
    }

    Q_INVOKABLE void removeMusicLocation(QString location);


Q_SIGNALS:

    void rootPathChanged(const QStringList &rootPath);

    void isDirtyChanged();

    void showNowPlayingBackgroundChanged();

    void showProgressInTaskBarChanged();

    void showSystemTrayIconChanged();

    void alwaysUseAbsolutePlaylistPathsChanged();

    void embeddedViewChanged();

    void initialViewIndexChanged();

    void initialFilesViewPathChanged();

    void playAtStartupChanged();

    void scanAtStartupChanged();

    void colorSchemeChanged();

    void useFavoriteStyleRatingsChanged();

public Q_SLOTS:

    void setRootPath(const QStringList &rootPath);

    void save();

    void cancel();

    void setShowNowPlayingBackground(bool showNowPlayingBackground);

    void setShowProgressInTaskBar(bool showProgressInTaskBar);

    void setShowSystemTrayIcon(bool showSystemTrayIcon);

    void setAlwaysUseAbsolutePlaylistPaths(bool alwaysUseAbsolutePlaylistPaths);

    void setEmbeddedView(ElisaUtils::PlayListEntryType embeddedView);

    void setInitialViewIndex(int initialViewIndex);

    void setInitialFilesViewPath(const QString &path);

    void setPlayAtStartup(bool playAtStartup);

    void setScanAtStartup(bool scanAtStartup);

    void setColorScheme(const QString &scheme);

    void setUseFavoriteStyleRatings(bool useFavoriteStyleRatings);

private Q_SLOTS:

    void configChanged();

private:

    void setDirty();

    QStringList mRootPath;

    QFileSystemWatcher mConfigFileWatcher;

    bool mIsDirty = false;

    bool mShowNowPlayingBackground = true;

    bool mShowProgressInTaskBar = true;

    bool mShowSystemTrayIcon = false;

    bool mAlwaysUseAbsolutePlaylistPaths = false;

    bool mPlayAtStartup = false;

    bool mScanAtStartup = true;

    bool mUseFavoriteStyleRatings = false;

    QString mColorScheme;

    ElisaUtils::PlayListEntryType mEmbeddedView = ElisaUtils::Unknown;

    int mInitialViewIndex = 2;

    QString mInitialFilesViewPath;
};

#endif
