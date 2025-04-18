/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ELISAAPPLICATION_H
#define ELISAAPPLICATION_H

#include "elisaLib_export.h"

#include "config-upnp-qt.h"

#include "elisautils.h"
#include "datatypes.h"

#include <QObject>
#include <QQmlEngine>
#include <QString>

#include <memory>

class QIcon;
class QAction;
class MusicListenersManager;
class MediaPlayList;
class MediaPlayListProxyModel;
class AudioWrapper;
class ManageAudioPlayer;
class ManageMediaPlayerControl;
class ManageHeaderBar;
class ElisaApplicationPrivate;
class KColorSchemeManager;
class QAbstractItemModel;

class ELISALIB_EXPORT ElisaApplication : public QObject
{

    Q_OBJECT

    QML_ELEMENT

    QML_SINGLETON

    Q_PROPERTY(ElisaUtils::PlayListEntryType embeddedView
               READ embeddedView
               NOTIFY embeddedViewChanged)

    Q_PROPERTY(MusicListenersManager *musicManager
               READ musicManager
               NOTIFY musicManagerChanged)

    Q_PROPERTY(MediaPlayList *mediaPlayList
               READ mediaPlayList
               NOTIFY mediaPlayListChanged)


    Q_PROPERTY(MediaPlayListProxyModel *mediaPlayListProxyModel
               READ mediaPlayListProxyModel
               NOTIFY mediaPlayListProxyModelChanged)


    Q_PROPERTY(AudioWrapper *audioPlayer
               READ audioPlayer
               NOTIFY audioPlayerChanged)

    Q_PROPERTY(ManageAudioPlayer *audioControl
               READ audioControl
               NOTIFY audioControlChanged)

    Q_PROPERTY(ManageMediaPlayerControl *playerControl
               READ playerControl
               NOTIFY playerControlChanged)

    Q_PROPERTY(ManageHeaderBar *manageHeaderBar
               READ manageHeaderBar
               NOTIFY manageHeaderBarChanged)

    Q_PROPERTY(bool showNowPlayingBackground
               READ showNowPlayingBackground
               NOTIFY showNowPlayingBackgroundChanged)

    Q_PROPERTY(bool showProgressOnTaskBar
               READ showProgressOnTaskBar
               NOTIFY showProgressOnTaskBarChanged)

    Q_PROPERTY(bool showSystemTrayIcon
               READ showSystemTrayIcon
               NOTIFY showSystemTrayIconChanged)

    Q_PROPERTY(bool useFavoriteStyleRatings
               READ useFavoriteStyleRatings
               NOTIFY useFavoriteStyleRatingsChanged)

    Q_PROPERTY(QAbstractItemModel* colorSchemesModel
               READ colorSchemesModel
               CONSTANT)

    Q_PROPERTY(QString activeColorSchemeName
               READ activeColorSchemeName
               WRITE setActiveColorSchemeName
               NOTIFY activeColorSchemeNameChanged)

    Q_PROPERTY(int initialViewIndex
               READ initialViewIndex
               NOTIFY initialViewIndexChanged)

    Q_PROPERTY(QString initialFilesViewPath
               READ initialFilesViewPath
               NOTIFY initialFilesViewPathChanged)

public:

    explicit ElisaApplication(QObject *parent = nullptr);

    ~ElisaApplication() override;

    Q_INVOKABLE QAction* action(const QString& name);

    Q_INVOKABLE void showInFolder(const QUrl &filePath);

    [[nodiscard]] MusicListenersManager *musicManager() const;

    [[nodiscard]] MediaPlayList *mediaPlayList() const;

    [[nodiscard]] MediaPlayListProxyModel *mediaPlayListProxyModel() const;

    [[nodiscard]] AudioWrapper *audioPlayer() const;

    [[nodiscard]] ManageAudioPlayer *audioControl() const;

    [[nodiscard]] ManageMediaPlayerControl *playerControl() const;

    [[nodiscard]] ManageHeaderBar *manageHeaderBar() const;

    [[nodiscard]] bool showNowPlayingBackground() const;

    [[nodiscard]] bool showProgressOnTaskBar() const;

    [[nodiscard]] bool showSystemTrayIcon() const;

    [[nodiscard]] bool useFavoriteStyleRatings() const;

    [[nodiscard]] ElisaUtils::PlayListEntryType embeddedView() const;

    int initialViewIndex() const;

    [[nodiscard]] QString initialFilesViewPath() const;

    QString activeColorSchemeName() const;
    void setActiveColorSchemeName(const QString &name);

Q_SIGNALS:

    void musicManagerChanged();

    void mediaPlayListChanged();

    void mediaPlayListProxyModelChanged();

    void audioPlayerChanged();

    void audioControlChanged();

    void playerControlChanged();

    void manageHeaderBarChanged();

    void enqueue(const DataTypes::EntryDataList &newEntries,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void initializationDone();

    void showNowPlayingBackgroundChanged();

    void showProgressOnTaskBarChanged();

    void showSystemTrayIconChanged();

    void useFavoriteStyleRatingsChanged();

    void commitDataRequest();

    void embeddedViewChanged();

    void initialViewIndexChanged();

    void initialFilesViewPathChanged();

    void activeColorSchemeNameChanged();

    void raisePlayer();

    // Actions

    void openAboutAppPage();

    void openAboutKDEPage();

    void configureElisa();

    void goBack();

    void togglePlaylist();

    void seek();

    void scrub();

    void nextTrack();

    void previousTrack();

    void playPause();

    void find();

    void togglePartyMode();

public Q_SLOTS:

    void reportBug();

    void configureShortcuts();

    bool openFiles(const QList<QUrl> &files);
    bool openFiles(const QList<QUrl> &files, const QString &workingDirectory);

    void activateActionRequested(const QString &actionName, const QVariant &parameter);

    void activateRequested(const QStringList &arguments, const QString &workingDirectory);

    void openRequested(const QList<QUrl> &uris);

    void initialize();

private Q_SLOTS:

    void configChanged();

private:

    void initializeModels();

    void initializePlayer();

    void setupActions();

    [[nodiscard]] DataTypes::EntryDataList checkFileListAndMakeAbsolute(const DataTypes::EntryDataList &filesList,
                                                              const QString &workingDirectory) const;

    QAbstractItemModel *colorSchemesModel();

    std::unique_ptr<ElisaApplicationPrivate> d;

};

#endif // ELISAAPPLICATION_H
