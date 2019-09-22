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
class QQmlEngine;
class ElisaApplicationPrivate;
class QSessionManager;

class ELISALIB_EXPORT ElisaApplication : public QObject
{

    Q_OBJECT

    Q_PROPERTY(DataTypes::EntryDataList arguments
               READ arguments
               WRITE setArguments
               NOTIFY argumentsChanged)

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

    Q_PROPERTY(bool showProgressOnTaskBar
               READ showProgressOnTaskBar
               NOTIFY showProgressOnTaskBarChanged)

    Q_PROPERTY(bool showSystemTrayIcon
               READ showSystemTrayIcon
               NOTIFY showSystemTrayIconChanged)

public:
    explicit ElisaApplication(QObject *parent = nullptr);

    ~ElisaApplication() override;

    Q_INVOKABLE QAction* action(const QString& name);

    Q_INVOKABLE QString iconName(const QIcon& icon);

    Q_INVOKABLE void installKeyEventFilter(QObject *object);

    bool eventFilter(QObject *object, QEvent *event) override;

    const DataTypes::EntryDataList &arguments() const;

    MusicListenersManager *musicManager() const;

    MediaPlayList *mediaPlayList() const;

    MediaPlayListProxyModel *mediaPlayListProxyModel() const;

    AudioWrapper *audioPlayer() const;

    ManageAudioPlayer *audioControl() const;

    ManageMediaPlayerControl *playerControl() const;

    ManageHeaderBar *manageHeaderBar() const;

    bool showProgressOnTaskBar() const;

    bool showSystemTrayIcon() const;

    ElisaUtils::PlayListEntryType embeddedView() const;

Q_SIGNALS:

    void argumentsChanged();

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

    void showProgressOnTaskBarChanged();

    void showSystemTrayIconChanged();

    void commitDataRequest(QSessionManager &manager);

    void embeddedViewChanged();

public Q_SLOTS:

    void appHelpActivated();

    void aboutApplication();

    void reportBug();

    void configureShortcuts();

    void configureElisa();

    void setArguments(const DataTypes::EntryDataList &newArguments);

    void activateActionRequested(const QString &actionName, const QVariant &parameter);

    void activateRequested(const QStringList &arguments, const QString &workingDirectory);

    void openRequested(const QList< QUrl > &uris);

    void initialize();

public:

    void setQmlEngine(QQmlEngine *engine);

private Q_SLOTS:

    void goBack();

    void find();

    void togglePlaylist();

    void seek();

    void scrub();

    void playPause();

    void configChanged();

private:

    void initializeModels();

    void initializePlayer();

    void setupActions(const QString &actionName);

    DataTypes::EntryDataList checkFileListAndMakeAbsolute(const DataTypes::EntryDataList &filesList,
                                                              const QString &workingDirectory) const;

    std::unique_ptr<ElisaApplicationPrivate> d;

};

#endif // ELISAAPPLICATION_H
