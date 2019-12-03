/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
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

#include "elisaapplication.h"

#include "musiclistenersmanager.h"

#include "mediaplaylist.h"
#include "audiowrapper.h"
#include "manageaudioplayer.h"
#include "managemediaplayercontrol.h"
#include "manageheaderbar.h"
#include "databaseinterface.h"

#include "elisa_settings.h"
#include <KConfigCore/KAuthorized>

#if defined KF5ConfigWidgets_FOUND && KF5ConfigWidgets_FOUND
#include <KConfigWidgets/KStandardAction>
#endif

#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
#include <KXmlGui/KActionCollection>
#include <KXmlGui/KAboutApplicationDialog>
#include <KXmlGui/KHelpMenu>
#include <KXmlGui/KBugReport>
#include <KXmlGui/KShortcutsDialog>
#endif

#if defined KF5KCMUtils_FOUND && KF5KCMUtils_FOUND
#include <KCMUtils/KCMultiDialog>
#endif

#include <KCoreAddons/KAboutData>

#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QPointer>
#include <QIcon>
#include <QAction>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QKeyEvent>
#include <QDebug>
#include <QFileSystemWatcher>

#include <memory>

class ElisaApplicationPrivate
{
public:

    explicit ElisaApplicationPrivate(QObject *parent)
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
        : mCollection(parent)
    #endif
    {
        Q_UNUSED(parent)

        auto configurationFileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        configurationFileName += QStringLiteral("/elisarc");
        Elisa::ElisaConfiguration::instance(configurationFileName);
        Elisa::ElisaConfiguration::self()->load();
        Elisa::ElisaConfiguration::self()->save();

        mConfigFileWatcher.addPath(configurationFileName);
    }

#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    KActionCollection mCollection;
#endif

    ElisaUtils::EntryDataList mArguments;

    std::unique_ptr<MusicListenersManager> mMusicManager;

    std::unique_ptr<MediaPlayList> mMediaPlayList;

    std::unique_ptr<AudioWrapper> mAudioWrapper;

    std::unique_ptr<ManageAudioPlayer> mAudioControl;

    std::unique_ptr<ManageMediaPlayerControl> mPlayerControl;

    std::unique_ptr<ManageHeaderBar> mManageHeaderBar;

    QQmlApplicationEngine *mEngine = nullptr;

    QFileSystemWatcher mConfigFileWatcher;

};

ElisaApplication::ElisaApplication(QObject *parent) : QObject(parent), d(std::make_unique<ElisaApplicationPrivate>(this))
{
    connect(Elisa::ElisaConfiguration::self(), &Elisa::ElisaConfiguration::configChanged,
            this, &ElisaApplication::configChanged);

    connect(&d->mConfigFileWatcher, &QFileSystemWatcher::fileChanged,
            this, &ElisaApplication::configChanged);
}

ElisaApplication::~ElisaApplication()
= default;

void ElisaApplication::setupActions(const QString &actionName)
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    if (actionName == QLatin1String("file_quit")) {
        auto quitAction = KStandardAction::quit(QCoreApplication::instance(), &QCoreApplication::quit, &d->mCollection);
        d->mCollection.addAction(actionName, quitAction);
    }

    if (actionName == QLatin1String("help_contents") && KAuthorized::authorizeAction(actionName)) {
        auto handBookAction = KStandardAction::helpContents(this, &ElisaApplication::appHelpActivated, &d->mCollection);
        d->mCollection.addAction(handBookAction->objectName(), handBookAction);
    }

    if (actionName == QLatin1String("help_report_bug") && KAuthorized::authorizeAction(actionName) && !KAboutData::applicationData().bugAddress().isEmpty()) {
        auto reportBugAction = KStandardAction::reportBug(this, &ElisaApplication::reportBug, &d->mCollection);
        d->mCollection.addAction(reportBugAction->objectName(), reportBugAction);
    }

    if (actionName == QLatin1String("help_about_app") && KAuthorized::authorizeAction(actionName)) {
        auto aboutAppAction = KStandardAction::aboutApp(this, &ElisaApplication::aboutApplication, this);
        d->mCollection.addAction(aboutAppAction->objectName(), aboutAppAction);
    }

    if (actionName == QLatin1String("options_configure") && KAuthorized::authorizeAction(actionName)) {
        auto preferencesAction = KStandardAction::preferences(this, &ElisaApplication::configureElisa, this);
        d->mCollection.addAction(preferencesAction->objectName(), preferencesAction);
    }

    if (actionName == QLatin1String("options_configure_keybinding") && KAuthorized::authorizeAction(actionName)) {
        auto keyBindingsAction = KStandardAction::keyBindings(this, &ElisaApplication::configureShortcuts, this);
        d->mCollection.addAction(keyBindingsAction->objectName(), keyBindingsAction);
    }

    if (actionName == QLatin1String("go_back") && KAuthorized::authorizeAction(actionName)) {
        auto goBackAction = KStandardAction::back(this, &ElisaApplication::goBack, this);
        d->mCollection.addAction(goBackAction->objectName(), goBackAction);
    }

    if (actionName == QLatin1String("toggle_playlist") && KAuthorized::authorizeAction(actionName)) {
        auto togglePlaylistAction = d->mCollection.addAction(actionName, this, &ElisaApplication::togglePlaylist);
        togglePlaylistAction->setShortcut(QKeySequence(Qt::Key_F9));
        togglePlaylistAction->setText(QStringLiteral("Toggle Playlist"));
    }

    if (actionName == QLatin1String("Seek") && KAuthorized::authorizeAction(actionName)) {
            auto seekAction = d->mCollection.addAction(actionName, this, &ElisaApplication::seek);
            d->mCollection.setDefaultShortcut(seekAction, QKeySequence(Qt::SHIFT + Qt::Key_Right));
    }

    if (actionName == QLatin1String("Scrub") && KAuthorized::authorizeAction(actionName)) {
            auto scrubAction = d->mCollection.addAction(actionName, this, &ElisaApplication::scrub);
            d->mCollection.setDefaultShortcut(scrubAction, QKeySequence(Qt::SHIFT + Qt::Key_Left));
    }

    if (actionName == QLatin1String("Play-Pause") && KAuthorized::authorizeAction(actionName)) {
            auto playPauseAction = d->mCollection.addAction(actionName, this, &ElisaApplication::playPause);
            d->mCollection.setDefaultShortcut(playPauseAction, QKeySequence(Qt::Key_Space));
    }

    if (actionName == QLatin1String("edit_find") && KAuthorized::authorizeAction(actionName)) {
        auto findAction = KStandardAction::find(this, &ElisaApplication::find, this);
        d->mCollection.addAction(findAction->objectName(), findAction);
    }

    d->mCollection.readSettings();
#endif
}

void ElisaApplication::setArguments(const ElisaUtils::EntryDataList &newArguments)
{
    if (d->mArguments == newArguments) {
        return;
    }

    d->mArguments = checkFileListAndMakeAbsolute(newArguments, QDir::currentPath());
    Q_EMIT argumentsChanged();

    if (!d->mArguments.isEmpty()) {
        Q_EMIT enqueue(d->mArguments, ElisaUtils::FileName,
                       ElisaUtils::PlayListEnqueueMode::AppendPlayList,
                       ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);
    }
}

void ElisaApplication::activateActionRequested(const QString &actionName, const QVariant &parameter)
{
    Q_UNUSED(actionName)
    Q_UNUSED(parameter)
}

void ElisaApplication::activateRequested(const QStringList &arguments, const QString &workingDirectory)
{
    if (arguments.size() > 1) {
        auto realArguments = ElisaUtils::EntryDataList{};

        bool isFirst = true;
        for (const auto &oneArgument : arguments) {
            if (isFirst) {
                isFirst = false;
                continue;
            }

            realArguments.push_back(ElisaUtils::EntryData{{}, {}, QUrl(oneArgument)});
        }

        Q_EMIT enqueue(checkFileListAndMakeAbsolute(realArguments, workingDirectory),
                       ElisaUtils::FileName,
                       ElisaUtils::PlayListEnqueueMode::AppendPlayList,
                       ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);
    }
}

void ElisaApplication::openRequested(const QList<QUrl> &uris)
{
    Q_UNUSED(uris)
}

void ElisaApplication::appHelpActivated()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("help:/")));
}

void ElisaApplication::aboutApplication()
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    static QPointer<QDialog> dialog;
    if (!dialog) {
        dialog = new KAboutApplicationDialog(KAboutData::applicationData(), nullptr);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    dialog->show();
#endif
}

void ElisaApplication::reportBug()
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    static QPointer<QDialog> dialog;
    if (!dialog) {
        dialog = new KBugReport(KAboutData::applicationData(), nullptr);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    dialog->show();
#endif
}

void ElisaApplication::configureShortcuts()
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    KShortcutsDialog dlg(KShortcutsEditor::AllActions, KShortcutsEditor::LetterShortcutsAllowed, nullptr);
    dlg.setModal(true);
    dlg.addCollection(&d->mCollection);
    dlg.configure();
#endif
}

void ElisaApplication::configureElisa()
{
    if (!d->mEngine)
    {
        return;
    }

    d->mEngine->load(QUrl(QStringLiteral("qrc:/qml/ElisaConfigurationDialog.qml")));
}

void ElisaApplication::goBack() {}

void ElisaApplication::find() {}

void ElisaApplication::togglePlaylist() {}

void ElisaApplication::seek() {}

void ElisaApplication::scrub() {}

void ElisaApplication::playPause() {}

void ElisaApplication::configChanged()
{
    auto currentConfiguration = Elisa::ElisaConfiguration::self();

    d->mConfigFileWatcher.addPath(currentConfiguration->config()->name());

    currentConfiguration->load();
    currentConfiguration->read();

    Q_EMIT showProgressOnTaskBarChanged();
}

ElisaUtils::EntryDataList ElisaApplication::checkFileListAndMakeAbsolute(const ElisaUtils::EntryDataList &filesList,
                                                                         const QString &workingDirectory) const
{
    auto filesToOpen = ElisaUtils::EntryDataList{};

    for (const auto &oneFile : filesList) {
        if (std::get<2>(oneFile).scheme().isEmpty() || std::get<2>(oneFile).isLocalFile()) {
            auto newFile = QFileInfo(std::get<2>(oneFile).toLocalFile());
            if (std::get<2>(oneFile).scheme().isEmpty()) {
                newFile = QFileInfo(std::get<2>(oneFile).toString());
            }

            if (newFile.isRelative()) {
                if (std::get<2>(oneFile).scheme().isEmpty()) {
                    newFile.setFile(workingDirectory, std::get<2>(oneFile).toString());
                } else {
                    newFile.setFile(workingDirectory, std::get<2>(oneFile).toLocalFile());
                }
            }

            if (newFile.exists()) {
                filesToOpen.push_back(ElisaUtils::EntryData{{}, {}, QUrl::fromLocalFile(newFile.canonicalFilePath())});
            }
        } else {
            filesToOpen.push_back(ElisaUtils::EntryData{{}, {}, std::get<2>(oneFile)});
        }
    }

    return filesToOpen;
}

void ElisaApplication::initialize()
{
    initializeModels();
    initializePlayer();

    Q_EMIT initializationDone();
}

void ElisaApplication::setQmlEngine(QQmlApplicationEngine *engine)
{
    d->mEngine = engine;
}

void ElisaApplication::initializeModels()
{
    d->mMusicManager = std::make_unique<MusicListenersManager>();
    Q_EMIT musicManagerChanged();

    d->mMediaPlayList = std::make_unique<MediaPlayList>();
    d->mMusicManager->subscribeForTracks(d->mMediaPlayList.get());
    Q_EMIT mediaPlayListChanged();

    d->mMusicManager->setElisaApplication(this);

    QObject::connect(this, &ElisaApplication::enqueue,
                     d->mMediaPlayList.get(), static_cast<void (MediaPlayList::*)(const ElisaUtils::EntryDataList&,
                                                                                  ElisaUtils::PlayListEntryType,
                                                                                  ElisaUtils::PlayListEnqueueMode,
                                                                                  ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayList::enqueue));
}

void ElisaApplication::initializePlayer()
{
    d->mAudioWrapper = std::make_unique<AudioWrapper>();
    Q_EMIT audioPlayerChanged();
    d->mAudioControl = std::make_unique<ManageAudioPlayer>();
    Q_EMIT audioControlChanged();
    d->mPlayerControl = std::make_unique<ManageMediaPlayerControl>();
    Q_EMIT playerControlChanged();
    d->mManageHeaderBar = std::make_unique<ManageHeaderBar>();
    Q_EMIT manageHeaderBarChanged();

    d->mAudioControl->setAlbumNameRole(MediaPlayList::AlbumRole);
    d->mAudioControl->setArtistNameRole(MediaPlayList::ArtistRole);
    d->mAudioControl->setTitleRole(MediaPlayList::TitleRole);
    d->mAudioControl->setUrlRole(MediaPlayList::ResourceRole);
    d->mAudioControl->setIsPlayingRole(MediaPlayList::IsPlayingRole);
    d->mAudioControl->setPlayListModel(d->mMediaPlayList.get());

    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::playerPlay, d->mAudioWrapper.get(), &AudioWrapper::play);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::playerPause, d->mAudioWrapper.get(), &AudioWrapper::pause);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::playerStop, d->mAudioWrapper.get(), &AudioWrapper::stop);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::seek, d->mAudioWrapper.get(), &AudioWrapper::seek);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::saveUndoPositionInAudioWrapper, d->mAudioWrapper.get(), &AudioWrapper::saveUndoPosition);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::restoreUndoPositionInAudioWrapper, d->mAudioWrapper.get(), &AudioWrapper::restoreUndoPosition);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::skipNextTrack, d->mMediaPlayList.get(), &MediaPlayList::skipNextTrack);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::sourceInError, d->mMediaPlayList.get(), &MediaPlayList::trackInError);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::sourceInError, d->mMusicManager.get(), &MusicListenersManager::playBackError);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::playerSourceChanged, d->mAudioWrapper.get(), &AudioWrapper::setSource);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::startedPlayingTrack,
                     d->mMusicManager->viewDatabase(), &DatabaseInterface::trackHasStartedPlaying);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::updateData, d->mMediaPlayList.get(), &MediaPlayList::setData);

    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::ensurePlay, d->mAudioControl.get(), &ManageAudioPlayer::ensurePlay);
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::playListFinished, d->mAudioControl.get(), &ManageAudioPlayer::playListFinished);
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::currentTrackChanged, d->mAudioControl.get(), &ManageAudioPlayer::setCurrentTrack);
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::clearPlayListPlayer, d->mAudioControl.get(), &ManageAudioPlayer::saveForUndoClearPlaylist);
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::undoClearPlayListPlayer, d->mAudioControl.get(), &ManageAudioPlayer::restoreForUndoClearPlaylist);

    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::playbackStateChanged,
                     d->mAudioControl.get(), &ManageAudioPlayer::setPlayerPlaybackState);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::statusChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerStatus);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::errorChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerError);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::durationChanged, d->mAudioControl.get(), &ManageAudioPlayer::setAudioDuration);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::seekableChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerIsSeekable);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::positionChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerPosition);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::currentPlayingForRadiosChanged, d->mAudioControl.get(), &ManageAudioPlayer::setCurrentPlayingForRadios);

    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::currentTrackChanged, d->mPlayerControl.get(), &ManageMediaPlayerControl::setCurrentTrack);
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::previousTrackChanged, d->mPlayerControl.get(), &ManageMediaPlayerControl::setPreviousTrack);
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::nextTrackChanged, d->mPlayerControl.get(), &ManageMediaPlayerControl::setNextTrack);

    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::playing, d->mPlayerControl.get(), &ManageMediaPlayerControl::playerPlaying);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::paused, d->mPlayerControl.get(), &ManageMediaPlayerControl::playerPausedOrStopped);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::stopped, d->mPlayerControl.get(), &ManageMediaPlayerControl::playerPausedOrStopped);

    d->mManageHeaderBar->setTitleRole(MediaPlayList::TitleRole);
    d->mManageHeaderBar->setAlbumRole(MediaPlayList::AlbumRole);
    d->mManageHeaderBar->setAlbumArtistRole(MediaPlayList::AlbumArtistRole);
    d->mManageHeaderBar->setArtistRole(MediaPlayList::ArtistRole);
    d->mManageHeaderBar->setFileNameRole(MediaPlayList::ResourceRole);
    d->mManageHeaderBar->setImageRole(MediaPlayList::ImageUrlRole);
    d->mManageHeaderBar->setDatabaseIdRole(MediaPlayList::DatabaseIdRole);
    d->mManageHeaderBar->setTrackTypeRole(MediaPlayList::ElementTypeRole);
    d->mManageHeaderBar->setAlbumIdRole(MediaPlayList::AlbumIdRole);
    d->mManageHeaderBar->setIsValidRole(MediaPlayList::IsValidRole);
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::currentTrackChanged, d->mManageHeaderBar.get(), &ManageHeaderBar::setCurrentTrack);

    if (!d->mArguments.isEmpty()) {
        Q_EMIT enqueue(d->mArguments, ElisaUtils::FileName,
                       ElisaUtils::PlayListEnqueueMode::AppendPlayList,
                       ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);
    }
}

QAction * ElisaApplication::action(const QString& name)
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    auto resultAction = d->mCollection.action(name);

    if (!resultAction) {
        setupActions(name);
        resultAction = d->mCollection.action(name);
    }

    return resultAction;
#else
    Q_UNUSED(name);

    return new QAction();
#endif
}

QString ElisaApplication::iconName(const QIcon& icon)
{
    return icon.name();
}

void ElisaApplication::installKeyEventFilter(QObject *object)
{
    if(!object) {
        return;
    }

    object->installEventFilter(this);
}

bool ElisaApplication::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);

    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    auto playPauseAction = d->mCollection.action(tr("Play-Pause"));
    if (keyEvent->key() == Qt::Key_Space && playPauseAction->shortcut()[0] == Qt::Key_Space) {
        return true;
    }

    return false;
}

const ElisaUtils::EntryDataList &ElisaApplication::arguments() const
{
    return d->mArguments;
}

MusicListenersManager *ElisaApplication::musicManager() const
{
    return d->mMusicManager.get();
}

MediaPlayList *ElisaApplication::mediaPlayList() const
{
    return d->mMediaPlayList.get();
}

AudioWrapper *ElisaApplication::audioPlayer() const
{
    return d->mAudioWrapper.get();
}

ManageAudioPlayer *ElisaApplication::audioControl() const
{
    return d->mAudioControl.get();
}

ManageMediaPlayerControl *ElisaApplication::playerControl() const
{
    return d->mPlayerControl.get();
}

ManageHeaderBar *ElisaApplication::manageHeaderBar() const
{
    return d->mManageHeaderBar.get();
}

bool ElisaApplication::showProgressOnTaskBar() const
{
    auto currentConfiguration = Elisa::ElisaConfiguration::self();

    return currentConfiguration->showProgressOnTaskBar();
}

#include "moc_elisaapplication.cpp"
