/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2012 (c) Aleix Pol Gonzalez <aleixpol@blue-systems.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisaapplication.h"

#include "musiclistenersmanager.h"

#include "mediaplaylist.h"
#include "mediaplaylistproxymodel.h"
#include "audiowrapper.h"
#include "manageaudioplayer.h"
#include "managemediaplayercontrol.h"
#include "manageheaderbar.h"
#include "databaseinterface.h"

#include "elisa_settings.h"
#include <KAuthorized>
#include <KLocalizedString>
#include <KStandardActions>

#if KFXmlGui_FOUND
#include <KActionCollection>
#include <KHelpMenu>
#include <KBugReport>
#include <KShortcutsDialog>
#endif

#include <KColorSchemeManager>
#include <KAboutData>

#if KFKIO_FOUND
#include <KIO/OpenFileManagerWindowJob>
#endif

#include <QQmlEngine>
#include <QQmlComponent>
#include <QGuiApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QPointer>
#include <QIcon>
#include <QAction>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QFileSystemWatcher>
#include <QKeyEvent>
#include <QFileSystemWatcher>
#include <QMimeDatabase>
#include <QMimeType>

using namespace Qt::Literals::StringLiterals;

class ElisaApplicationPrivate
{
public:

    explicit ElisaApplicationPrivate(QObject *parent)
#if KFXmlGui_FOUND
        : mCollection(parent)
#endif
    {
        Q_UNUSED(parent)
    }

#if KFXmlGui_FOUND
    KActionCollection mCollection;
#endif

    std::unique_ptr<MusicListenersManager> mMusicManager;

    std::unique_ptr<MediaPlayList> mMediaPlayList;

    std::unique_ptr<MediaPlayListProxyModel> mMediaPlayListProxyModel;

    std::unique_ptr<AudioWrapper> mAudioWrapper;

    std::unique_ptr<ManageAudioPlayer> mAudioControl;

    std::unique_ptr<ManageMediaPlayerControl> mPlayerControl;

    std::unique_ptr<ManageHeaderBar> mManageHeaderBar;

    QFileSystemWatcher mConfigFileWatcher;

    KColorSchemeManager *mSchemes = nullptr;
};

ElisaApplication::ElisaApplication(QObject *parent) : QObject(parent), d(std::make_unique<ElisaApplicationPrivate>(this))
{
    auto configurationFileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    configurationFileName += QStringLiteral("/elisarc");
    Elisa::ElisaConfiguration::instance(configurationFileName);
    Elisa::ElisaConfiguration::self()->load();
    Elisa::ElisaConfiguration::self()->save();
    d->mSchemes = KColorSchemeManager::instance();

    d->mConfigFileWatcher.addPath(Elisa::ElisaConfiguration::self()->config()->name());

    connect(Elisa::ElisaConfiguration::self(), &Elisa::ElisaConfiguration::configChanged,
            this, &ElisaApplication::configChanged);

    connect(&d->mConfigFileWatcher, &QFileSystemWatcher::fileChanged,
            this, &ElisaApplication::configChanged);

    connect(static_cast<QGuiApplication*>(QCoreApplication::instance()), &QGuiApplication::commitDataRequest,
            this, &ElisaApplication::commitDataRequest);

    setupActions();
}

ElisaApplication::~ElisaApplication()
= default;

QAbstractItemModel *ElisaApplication::colorSchemesModel()
{
    return d->mSchemes->model();
}

QString ElisaApplication::activeColorSchemeName() const
{
    return d->mSchemes->activeSchemeName();
}

void ElisaApplication::setActiveColorSchemeName(const QString &name)
{
    d->mSchemes->activateScheme(d->mSchemes->indexForScheme(name));
    Q_EMIT activeColorSchemeNameChanged();
}

void ElisaApplication::setupActions()
{
#if KFXmlGui_FOUND
    QString actionName;

    actionName = u"file_quit"_s;
    auto quitAction = KStandardActions::quit(QCoreApplication::instance(), &QCoreApplication::quit, &d->mCollection);
    d->mCollection.addAction(actionName, quitAction);

    actionName = u"help_report_bug"_s;
    if (KAuthorized::authorizeAction(actionName) && !KAboutData::applicationData().bugAddress().isEmpty()) {
        auto reportBugAction = KStandardActions::reportBug(this, &ElisaApplication::reportBug, &d->mCollection);
        d->mCollection.addAction(reportBugAction->objectName(), reportBugAction);
    }

    actionName = u"help_about_app"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto aboutAppAction = KStandardActions::aboutApp(this, &ElisaApplication::openAboutAppPage, this);
        d->mCollection.addAction(aboutAppAction->objectName(), aboutAppAction);
    }

    actionName = u"help_about_kde"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto aboutKDEAction = KStandardActions::aboutKDE(this, &ElisaApplication::openAboutKDEPage, this);
        d->mCollection.addAction(aboutKDEAction->objectName(), aboutKDEAction);
    }

    actionName = u"options_configure"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto preferencesAction = KStandardActions::preferences(this, &ElisaApplication::configureElisa, this);
        d->mCollection.addAction(preferencesAction->objectName(), preferencesAction);
    }

    actionName = u"options_configure_keybinding"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto keyBindingsAction = KStandardActions::keyBindings(this, &ElisaApplication::configureShortcuts, this);
        d->mCollection.addAction(keyBindingsAction->objectName(), keyBindingsAction);
    }

    actionName = u"go_back"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto goBackAction = KStandardActions::back(this, &ElisaApplication::goBack, this);
        d->mCollection.addAction(goBackAction->objectName(), goBackAction);
    }

    actionName = u"toggle_playlist"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto togglePlaylistAction = d->mCollection.addAction(actionName, this, &ElisaApplication::togglePlaylist);
        togglePlaylistAction->setShortcut(QKeySequence(Qt::Key_F9));
        togglePlaylistAction->setText(i18nc("@action", "Toggle Playlist"));
    }

    actionName = u"Seek"_s;
    if (KAuthorized::authorizeAction(actionName)) {
            auto seekAction = d->mCollection.addAction(actionName, this, &ElisaApplication::seek);
            seekAction->setText(i18nc("@action", "Seek forward 10 seconds"));
            d->mCollection.setDefaultShortcut(seekAction, QKeySequence(Qt::SHIFT | Qt::Key_Right));
    }

    actionName = u"Scrub"_s;
    if (KAuthorized::authorizeAction(actionName)) {
            auto scrubAction = d->mCollection.addAction(actionName, this, &ElisaApplication::scrub);
            scrubAction->setText(i18nc("@action", "Scrub backwards 10 seconds"));
            d->mCollection.setDefaultShortcut(scrubAction, QKeySequence(Qt::SHIFT | Qt::Key_Left));
    }

    actionName = u"NextTrack"_s;
    if (KAuthorized::authorizeAction(actionName)) {
            auto nextTrackAction = d->mCollection.addAction(actionName, this, &ElisaApplication::nextTrack);
            nextTrackAction->setText(i18nc("@action", "Go to next track"));
            d->mCollection.setDefaultShortcut(nextTrackAction, QKeySequence(Qt::CTRL | Qt::Key_Right));
    }

    actionName = u"PreviousTrack"_s;
    if (KAuthorized::authorizeAction(actionName)) {
            auto previousTrackAction = d->mCollection.addAction(actionName, this, &ElisaApplication::previousTrack);
            previousTrackAction->setText(i18nc("@action", "Go to previous track"));
            d->mCollection.setDefaultShortcut(previousTrackAction, QKeySequence(Qt::CTRL | Qt::Key_Left));
    }

    actionName = u"Play-Pause"_s;
    if (KAuthorized::authorizeAction(actionName)) {
            auto playPauseAction = d->mCollection.addAction(actionName, this, &ElisaApplication::playPause);
            playPauseAction->setText(i18nc("@action", "Play/pause"));
            d->mCollection.setDefaultShortcut(playPauseAction, QKeySequence(Qt::Key_Space));
    }

    actionName = u"edit_find"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto findAction = KStandardActions::find(this, &ElisaApplication::find, this);
        d->mCollection.addAction(findAction->objectName(), findAction);
    }

    actionName = u"togglePartyMode"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto togglePartyModeAction = d->mCollection.addAction(actionName, this, &ElisaApplication::togglePartyMode);
        togglePartyModeAction->setText(i18nc("@action", "Toggle Party Mode"));
        d->mCollection.setDefaultShortcut(togglePartyModeAction, QKeySequence(Qt::Key_F11));
        d->mCollection.addAction(togglePartyModeAction->objectName(), togglePartyModeAction);
    }

    d->mCollection.readSettings();
#else
#endif
}

bool ElisaApplication::openFiles(const QList<QUrl> &files)
{
    return openFiles(files, QDir::currentPath());
}

bool ElisaApplication::openFiles(const QList<QUrl> &files, const QString &workingDirectory)
{
    auto audioFiles = DataTypes::EntryDataList{};
    const QMimeDatabase mimeDB;
    for (const auto &file : files) {
        const QMimeType mime = mimeDB.mimeTypeForUrl(file);
        if (ElisaUtils::isPlayList(mime)) {
            d->mMediaPlayListProxyModel->loadPlayList(file);
        } else if (mime.name().startsWith(QStringLiteral("audio/"))) {
            audioFiles.push_back(DataTypes::EntryData{{{DataTypes::ElementTypeRole, ElisaUtils::FileName},
                            {DataTypes::ResourceRole, file}}, {}, {}});
        }
    }
    auto filesToOpen = checkFileListAndMakeAbsolute(audioFiles, workingDirectory);
    if (!filesToOpen.isEmpty()) {
        Q_EMIT enqueue(filesToOpen,
                       ElisaUtils::PlayListEnqueueMode::AppendPlayList,
                       ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);
    }
    return (audioFiles.count() == filesToOpen.count());
}

void ElisaApplication::activateActionRequested(const QString &actionName, const QVariant &parameter)
{
    Q_UNUSED(actionName)
    Q_UNUSED(parameter)
}

void ElisaApplication::activateRequested(const QStringList &arguments, const QString &workingDirectory)
{
    Q_EMIT raisePlayer();
    if (arguments.size() > 1) {
        QList<QUrl> urls;
        bool isFirst = true;
        for (const auto &oneArgument : arguments) {
            if (isFirst) {
                isFirst = false;
                continue;
            }
            urls.push_back(QUrl::fromUserInput(oneArgument));
        }
        openFiles(urls, workingDirectory);
    }
}

void ElisaApplication::openRequested(const QList<QUrl> &uris)
{
    Q_EMIT raisePlayer();
    openFiles(uris);
}

void ElisaApplication::reportBug()
{
#if KFXmlGui_FOUND
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
#if KFXmlGui_FOUND
    KShortcutsDialog dlg(KShortcutsEditor::ApplicationAction, KShortcutsEditor::LetterShortcutsAllowed, nullptr);
    dlg.setModal(true);
    dlg.addCollection(&d->mCollection);
    dlg.configure();
#endif
}

void ElisaApplication::configChanged()
{
    auto currentConfiguration = Elisa::ElisaConfiguration::self();

    d->mConfigFileWatcher.addPath(currentConfiguration->config()->name());

    currentConfiguration->load();
    currentConfiguration->read();

    Q_EMIT showNowPlayingBackgroundChanged();
    Q_EMIT showProgressOnTaskBarChanged();
    Q_EMIT showSystemTrayIconChanged();
    Q_EMIT useFavoriteStyleRatingsChanged();
    Q_EMIT embeddedViewChanged();
    Q_EMIT initialViewIndexChanged();
    Q_EMIT initialFilesViewPathChanged();
}

DataTypes::EntryDataList ElisaApplication::checkFileListAndMakeAbsolute(const DataTypes::EntryDataList &filesList,
                                                                         const QString &workingDirectory) const
{
    auto filesToOpen = DataTypes::EntryDataList{};

    for (const auto &oneFile : filesList) {
        auto trackUrl = oneFile.musicData[DataTypes::ResourceRole].toUrl();
        if (trackUrl.scheme().isEmpty() || trackUrl.isLocalFile()) {
            auto newFile = QFileInfo(trackUrl.toLocalFile());
            if (trackUrl.scheme().isEmpty()) {
                newFile = QFileInfo(trackUrl.toString());
            }

            if (newFile.isRelative()) {
                if (trackUrl.scheme().isEmpty()) {
                    newFile.setFile(workingDirectory, trackUrl.toString());
                } else {
                    newFile.setFile(workingDirectory, trackUrl.toLocalFile());
                }
            }

            if (newFile.exists()) {
                auto trackData = oneFile.musicData;
                trackData[DataTypes::ResourceRole] = QUrl::fromLocalFile(newFile.canonicalFilePath());
                filesToOpen.push_back({trackData, oneFile.title, {}});
            }
        } else {
            filesToOpen.push_back(oneFile);
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

void ElisaApplication::initializeModels()
{
    d->mMusicManager = std::make_unique<MusicListenersManager>();
    Q_EMIT musicManagerChanged();

    d->mMediaPlayList = std::make_unique<MediaPlayList>();
    d->mMusicManager->subscribeForTracks(d->mMediaPlayList.get());
    Q_EMIT mediaPlayListChanged();

    d->mMediaPlayListProxyModel = std::make_unique<MediaPlayListProxyModel>();
    d->mMediaPlayListProxyModel->setPlayListModel(d->mMediaPlayList.get());
    Q_EMIT mediaPlayListProxyModelChanged();

    d->mMusicManager->setElisaApplication(this);

    QObject::connect(this, &ElisaApplication::enqueue,
                     d->mMediaPlayListProxyModel.get(), static_cast<void (MediaPlayListProxyModel::*)(const DataTypes::EntryDataList&,
                                                                                            ElisaUtils::PlayListEnqueueMode,
                                                                                            ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
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
    d->mAudioControl->setPlayListModel(d->mMediaPlayListProxyModel.get());

    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::playerPlay, d->mAudioWrapper.get(), &AudioWrapper::play);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::playerPause, d->mAudioWrapper.get(), &AudioWrapper::pause);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::playerStop, d->mAudioWrapper.get(), &AudioWrapper::stop);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::seek, d->mAudioWrapper.get(), &AudioWrapper::seek);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::saveUndoPositionInAudioWrapper, d->mAudioWrapper.get(), &AudioWrapper::saveUndoPosition);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::restoreUndoPositionInAudioWrapper, d->mAudioWrapper.get(), &AudioWrapper::restoreUndoPosition);

    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::skipNextTrack, d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::skipNextTrack);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::skipPreviousTrack, d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::skipPreviousTrack);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::sourceInError, d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::trackInError);

    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::sourceInError, d->mMusicManager.get(), &MusicListenersManager::playBackError);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::playerSourceChanged, d->mAudioWrapper.get(), &AudioWrapper::setSource);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::startedPlayingTrack,
                     d->mMusicManager->viewDatabase(), &DatabaseInterface::trackHasStartedPlaying);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::finishedPlayingTrack,
                     d->mMusicManager->viewDatabase(), &DatabaseInterface::trackHasFinishedPlaying);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::updateData, d->mMediaPlayList.get(), &MediaPlayList::setData);

    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::ensurePlay, d->mAudioControl.get(), &ManageAudioPlayer::ensurePlay);
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::requestPlay, d->mAudioControl.get(), &ManageAudioPlayer::requestPlay);
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::playListFinished, d->mAudioControl.get(), &ManageAudioPlayer::playListFinished);
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::currentTrackChanged, d->mAudioControl.get(), &ManageAudioPlayer::setCurrentTrack);
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::clearPlayListPlayer, d->mAudioControl.get(), &ManageAudioPlayer::saveForUndoClearPlaylist);
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::undoClearPlayListPlayer, d->mAudioControl.get(), &ManageAudioPlayer::restoreForUndoClearPlaylist);
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::seek, d->mAudioWrapper.get(), &AudioWrapper::seek);

    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::playbackStateChanged,
                     d->mAudioControl.get(), &ManageAudioPlayer::setPlayerPlaybackState);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::statusChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerStatus);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::errorChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerError);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::durationChanged, d->mAudioControl.get(), &ManageAudioPlayer::setAudioDuration);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::seekableChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerIsSeekable);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::positionChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerPosition);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::currentPlayingForRadiosChanged, d->mAudioControl.get(), &ManageAudioPlayer::setCurrentPlayingForRadios);

    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::currentTrackChanged, d->mPlayerControl.get(), &ManageMediaPlayerControl::setCurrentTrack);
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::previousTrackChanged, d->mPlayerControl.get(), &ManageMediaPlayerControl::setPreviousTrack);
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::nextTrackChanged, d->mPlayerControl.get(), &ManageMediaPlayerControl::setNextTrack);

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
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::currentTrackChanged, d->mManageHeaderBar.get(), &ManageHeaderBar::setCurrentTrack);
    QObject::connect(d->mMediaPlayListProxyModel.get(), &MediaPlayListProxyModel::currentTrackDataChanged, d->mManageHeaderBar.get(), &ManageHeaderBar::updateCurrentTrackData);
}

QAction * ElisaApplication::action(const QString& name)
{
#if KFXmlGui_FOUND
    auto resultAction = d->mCollection.action(name);

    return resultAction;
#else
    Q_UNUSED(name);

    return new QAction();
#endif
}

void ElisaApplication::showInFolder(const QUrl &filePath)
{
#if KFKIO_FOUND
    KIO::highlightInFileManager({filePath});
#else
    Q_UNUSED(filePath)
#endif
}

MusicListenersManager *ElisaApplication::musicManager() const
{
    return d->mMusicManager.get();
}

MediaPlayList *ElisaApplication::mediaPlayList() const
{
    return d->mMediaPlayList.get();
}

MediaPlayListProxyModel *ElisaApplication::mediaPlayListProxyModel() const
{
    return d->mMediaPlayListProxyModel.get();
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


bool ElisaApplication::showNowPlayingBackground() const
{
    auto currentConfiguration = Elisa::ElisaConfiguration::self();
    return currentConfiguration->showNowPlayingBackground();
}

bool ElisaApplication::showProgressOnTaskBar() const
{
    auto currentConfiguration = Elisa::ElisaConfiguration::self();

    return currentConfiguration->showProgressOnTaskBar();
}

bool ElisaApplication::showSystemTrayIcon() const
{
    auto currentConfiguration = Elisa::ElisaConfiguration::self();

    return currentConfiguration->showSystemTrayIcon();
}

bool ElisaApplication::useFavoriteStyleRatings() const
{
    auto currentConfiguration = Elisa::ElisaConfiguration::self();

    return currentConfiguration->useFavoriteStyleRatings();
}

ElisaUtils::PlayListEntryType ElisaApplication::embeddedView() const
{
    ElisaUtils::PlayListEntryType result = ElisaUtils::Unknown;

    switch (Elisa::ElisaConfiguration::self()->embeddedView())
    {
    case 0:
        result = ElisaUtils::Unknown;
        break;
    case 1:
        result = ElisaUtils::Album;
        break;
    case 2:
        result = ElisaUtils::Artist;
        break;
    case 3:
        result = ElisaUtils::Genre;
        break;
    }

    return result;
}

int ElisaApplication::initialViewIndex() const
{
    int result = Elisa::ElisaConfiguration::initialView();

    return result;
}

QString ElisaApplication::initialFilesViewPath() const
{
    return Elisa::ElisaConfiguration::initialFilesViewPath();
}


#include "moc_elisaapplication.cpp"
