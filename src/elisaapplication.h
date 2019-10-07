/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ELISAAPPLICATION_H
#define ELISAAPPLICATION_H

#include "elisaLib_export.h"

#include "config-upnp-qt.h"

#include "elisautils.h"

#include <QObject>
#include <QString>

#include <memory>

class QIcon;
class QAction;
class MusicListenersManager;
class MediaPlayList;
class AudioWrapper;
class ManageAudioPlayer;
class ManageMediaPlayerControl;
class ManageHeaderBar;
class QQmlApplicationEngine;
class ElisaApplicationPrivate;

class ELISALIB_EXPORT ElisaApplication : public QObject
{

    Q_OBJECT

    Q_PROPERTY(ElisaUtils::EntryDataList arguments
               READ arguments
               WRITE setArguments
               NOTIFY argumentsChanged)

    Q_PROPERTY(MusicListenersManager *musicManager
               READ musicManager
               NOTIFY musicManagerChanged)

    Q_PROPERTY(MediaPlayList *mediaPlayList
               READ mediaPlayList
               NOTIFY mediaPlayListChanged)

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

public:
    explicit ElisaApplication(QObject *parent = nullptr);

    ~ElisaApplication() override;

    Q_INVOKABLE QAction* action(const QString& name);

    Q_INVOKABLE QString iconName(const QIcon& icon);

    Q_INVOKABLE void installKeyEventFilter(QObject *object);

    bool eventFilter(QObject *object, QEvent *event) override;

    const ElisaUtils::EntryDataList &arguments() const;

    MusicListenersManager *musicManager() const;

    MediaPlayList *mediaPlayList() const;

    AudioWrapper *audioPlayer() const;

    ManageAudioPlayer *audioControl() const;

    ManageMediaPlayerControl *playerControl() const;

    ManageHeaderBar *manageHeaderBar() const;

    bool showProgressOnTaskBar() const;

Q_SIGNALS:

    void argumentsChanged();

    void musicManagerChanged();

    void mediaPlayListChanged();

    void audioPlayerChanged();

    void audioControlChanged();

    void playerControlChanged();

    void manageHeaderBarChanged();

    void enqueue(const ElisaUtils::EntryDataList &newEntries,
                 ElisaUtils::PlayListEntryType databaseIdType,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void initializationDone();

    void showProgressOnTaskBarChanged();

public Q_SLOTS:

    void appHelpActivated();

    void aboutApplication();

    void reportBug();

    void configureShortcuts();

    void configureElisa();

    void setArguments(const ElisaUtils::EntryDataList &newArguments);

    void activateActionRequested(const QString &actionName, const QVariant &parameter);

    void activateRequested(const QStringList &arguments, const QString &workingDirectory);

    void openRequested(const QList< QUrl > &uris);

    void initialize();

public:

    void setQmlEngine(QQmlApplicationEngine *engine);

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

    ElisaUtils::EntryDataList checkFileListAndMakeAbsolute(const ElisaUtils::EntryDataList &filesList,
                                                              const QString &workingDirectory) const;

    std::unique_ptr<ElisaApplicationPrivate> d;

};

#endif // ELISAAPPLICATION_H
