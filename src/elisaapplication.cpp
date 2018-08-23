/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "elisaapplication.h"

#include "musiclistenersmanager.h"
#include "models/allalbumsproxymodel.h"
#include "models/alltracksproxymodel.h"
#include "models/allartistsproxymodel.h"
#include "models/singleartistproxymodel.h"
#include "models/singlealbumproxymodel.h"

#if defined KF5KIO_FOUND && KF5KIO_FOUND
#include "models/filebrowserproxymodel.h"
#endif

#include "mediaplaylist.h"
#include "audiowrapper.h"
#include "manageaudioplayer.h"
#include "managemediaplayercontrol.h"
#include "manageheaderbar.h"

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

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QPointer>
#include <QIcon>
#include <QAction>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

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
    }

#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    KActionCollection mCollection;
#endif

    QStringList mArguments;

    std::unique_ptr<MusicListenersManager> mMusicManager;

    std::unique_ptr<AllAlbumsProxyModel> mAllAlbumsProxyModel;

    std::unique_ptr<AllArtistsProxyModel> mAllArtistsProxyModel;

    std::unique_ptr<AllTracksProxyModel> mAllTracksProxyModel;

    std::unique_ptr<AllArtistsProxyModel> mAllGenresProxyModel;

    std::unique_ptr<AllArtistsProxyModel> mAllComposersProxyModel;

    std::unique_ptr<AllArtistsProxyModel> mAllLyricistsProxyModel;

    std::unique_ptr<SingleArtistProxyModel> mSingleArtistProxyModel;

    std::unique_ptr<SingleAlbumProxyModel> mSingleAlbumProxyModel;

#if defined KF5KIO_FOUND && KF5KIO_FOUND
    std::unique_ptr<FileBrowserProxyModel> mFileBrowserProxyModel;
#endif

    std::unique_ptr<MediaPlayList> mMediaPlayList;

    std::unique_ptr<AudioWrapper> mAudioWrapper;

    std::unique_ptr<ManageAudioPlayer> mAudioControl;

    std::unique_ptr<ManageMediaPlayerControl> mPlayerControl;

    std::unique_ptr<ManageHeaderBar> mManageHeaderBar;

};

ElisaApplication::ElisaApplication(QObject *parent) : QObject(parent), d(std::make_unique<ElisaApplicationPrivate>(this))
{
}

ElisaApplication::~ElisaApplication()
= default;

void ElisaApplication::setupActions(const QString &actionName)
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    if (actionName == QStringLiteral("file_quit")) {
        auto quitAction = KStandardAction::quit(QCoreApplication::instance(), &QCoreApplication::quit, &d->mCollection);
        d->mCollection.addAction(actionName, quitAction);
    }

    if (actionName == QStringLiteral("help_contents") && KAuthorized::authorizeAction(actionName)) {
        auto handBookAction = KStandardAction::helpContents(this, &ElisaApplication::appHelpActivated, &d->mCollection);
        d->mCollection.addAction(handBookAction->objectName(), handBookAction);
    }

    if (actionName == QStringLiteral("help_report_bug") && KAuthorized::authorizeAction(actionName) && !KAboutData::applicationData().bugAddress().isEmpty()) {
        auto reportBugAction = KStandardAction::reportBug(this, &ElisaApplication::reportBug, &d->mCollection);
        d->mCollection.addAction(reportBugAction->objectName(), reportBugAction);
    }

    if (actionName == QStringLiteral("help_about_app") && KAuthorized::authorizeAction(actionName)) {
        auto aboutAppAction = KStandardAction::aboutApp(this, &ElisaApplication::aboutApplication, this);
        d->mCollection.addAction(aboutAppAction->objectName(), aboutAppAction);
    }

    if (actionName == QStringLiteral("options_configure") && KAuthorized::authorizeAction(actionName)) {
        auto preferencesAction = KStandardAction::preferences(this, &ElisaApplication::configureElisa, this);
        d->mCollection.addAction(preferencesAction->objectName(), preferencesAction);
    }

    if (actionName == QStringLiteral("options_configure_keybinding") && KAuthorized::authorizeAction(actionName)) {
        auto keyBindingsAction = KStandardAction::keyBindings(this, &ElisaApplication::configureShortcuts, this);
        d->mCollection.addAction(keyBindingsAction->objectName(), keyBindingsAction);
    }

    if (actionName == QStringLiteral("go_back") && KAuthorized::authorizeAction(actionName)) {
        auto goBackAction = KStandardAction::back(this, &ElisaApplication::goBack, this);
        d->mCollection.addAction(goBackAction->objectName(), goBackAction);
    }

    if (actionName == QStringLiteral("toggle_playlist") && KAuthorized::authorizeAction(actionName)) {
        auto togglePlaylistAction = d->mCollection.addAction(actionName, this, &ElisaApplication::togglePlaylist);
        togglePlaylistAction->setShortcut(QKeySequence(Qt::Key_F9));
        togglePlaylistAction->setText(QStringLiteral("Toggle Playlist"));
    }

    if (actionName == QStringLiteral("edit_find") && KAuthorized::authorizeAction(actionName)) {
        auto findAction = KStandardAction::find(this, &ElisaApplication::find, this);
        d->mCollection.addAction(findAction->objectName(), findAction);
    }

    d->mCollection.readSettings();
#endif
}

void ElisaApplication::setArguments(const QStringList &newArguments)
{
    if (d->mArguments == newArguments) {
        return;
    }

    d->mArguments = checkFileListAndMakeAbsolute(newArguments, QDir::currentPath());
    Q_EMIT argumentsChanged();

    if (!d->mArguments.isEmpty()) {
        Q_EMIT enqueue(d->mArguments);
    }
}

void ElisaApplication::activateActionRequested(const QString &actionName, const QVariant &parameter)
{
    Q_UNUSED(actionName)
    Q_UNUSED(parameter)
}

void ElisaApplication::activateRequested(const QStringList &arguments, const QString &workingDirectory)
{
    auto realArguments = arguments;
    if (realArguments.size() > 1) {
        realArguments.removeFirst();
        Q_EMIT enqueue(checkFileListAndMakeAbsolute(realArguments, workingDirectory));
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
#if defined KF5KCMUtils_FOUND && KF5KCMUtils_FOUND
    KCMultiDialog configurationDialog;

    configurationDialog.addModule(QStringLiteral("kcm_elisa_local_file"));
    configurationDialog.setModal(true);
    configurationDialog.exec();
#endif
}

void ElisaApplication::goBack() {}

void ElisaApplication::find() {}

void ElisaApplication::togglePlaylist() {}

QStringList ElisaApplication::checkFileListAndMakeAbsolute(const QStringList &filesList, const QString &workingDirectory) const
{
    QStringList filesToOpen;

    for (const auto &oneFile : filesList) {
        auto newFile = QFileInfo(oneFile);

        if (newFile.isRelative()) {
            newFile = QFileInfo(workingDirectory + QStringLiteral("/") + oneFile);
        }

        if (newFile.exists()) {
            filesToOpen.push_back(newFile.canonicalFilePath());
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
    d->mAllAlbumsProxyModel = std::make_unique<AllAlbumsProxyModel>();
    Q_EMIT allAlbumsProxyModelChanged();
    d->mAllArtistsProxyModel = std::make_unique<AllArtistsProxyModel>();
    Q_EMIT allArtistsProxyModelChanged();
    d->mAllGenresProxyModel = std::make_unique<AllArtistsProxyModel>();
    Q_EMIT allGenresProxyModelChanged();
    d->mAllComposersProxyModel = std::make_unique<AllArtistsProxyModel>();
    Q_EMIT allComposersProxyModelChanged();
    d->mAllLyricistsProxyModel = std::make_unique<AllArtistsProxyModel>();
    Q_EMIT allLyricistsProxyModelChanged();
    d->mAllTracksProxyModel = std::make_unique<AllTracksProxyModel>();
    Q_EMIT allTracksProxyModelChanged();
    d->mSingleArtistProxyModel = std::make_unique<SingleArtistProxyModel>();
    Q_EMIT singleArtistProxyModelChanged();
    d->mSingleAlbumProxyModel = std::make_unique<SingleAlbumProxyModel>();
    Q_EMIT singleAlbumProxyModelChanged();

#if defined KF5KIO_FOUND && KF5KIO_FOUND
    d->mFileBrowserProxyModel = std::make_unique<FileBrowserProxyModel>();
    Q_EMIT fileBrowserProxyModelChanged();
#endif

    d->mMediaPlayList = std::make_unique<MediaPlayList>();
    Q_EMIT mediaPlayListChanged();

    d->mMusicManager->setElisaApplication(this);

    d->mMediaPlayList->setMusicListenersManager(d->mMusicManager.get());
    QObject::connect(this, &ElisaApplication::enqueue, d->mMediaPlayList.get(), &MediaPlayList::enqueueAndPlay);

    d->mAllAlbumsProxyModel->setSourceModel(d->mMusicManager->allAlbumsModel());
    d->mAllArtistsProxyModel->setSourceModel(d->mMusicManager->allArtistsModel());
    d->mAllGenresProxyModel->setSourceModel(d->mMusicManager->allGenresModel());
    d->mAllComposersProxyModel->setSourceModel(d->mMusicManager->allComposersModel());
    d->mAllLyricistsProxyModel->setSourceModel(d->mMusicManager->allLyricistsModel());
    d->mAllTracksProxyModel->setSourceModel(d->mMusicManager->allTracksModel());
    d->mSingleArtistProxyModel->setSourceModel(d->mMusicManager->allAlbumsModel());
    d->mSingleAlbumProxyModel->setSourceModel(d->mMusicManager->albumModel());

    QObject::connect(d->mAllAlbumsProxyModel.get(), &AllAlbumsProxyModel::albumToEnqueue,
                     d->mMediaPlayList.get(), static_cast<void (MediaPlayList::*)(const QList<MusicAlbum> &,
                                                                         ElisaUtils::PlayListEnqueueMode,
                                                                         ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayList::enqueue));

    QObject::connect(d->mAllArtistsProxyModel.get(), &AllArtistsProxyModel::artistToEnqueue,
                     d->mMediaPlayList.get(), &MediaPlayList::enqueueArtists);

    QObject::connect(d->mAllTracksProxyModel.get(), &AllTracksProxyModel::trackToEnqueue,
                     d->mMediaPlayList.get(), static_cast<void (MediaPlayList::*)(const QList<MusicAudioTrack> &,
                                                                         ElisaUtils::PlayListEnqueueMode,
                                                                         ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayList::enqueue));

    QObject::connect(d->mSingleArtistProxyModel.get(), &SingleArtistProxyModel::albumToEnqueue,
                     d->mMediaPlayList.get(), static_cast<void (MediaPlayList::*)(const QList<MusicAlbum> &,
                                                                         ElisaUtils::PlayListEnqueueMode,
                                                                         ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayList::enqueue));

    QObject::connect(d->mSingleAlbumProxyModel.get(), &SingleAlbumProxyModel::trackToEnqueue,
                     d->mMediaPlayList.get(), static_cast<void (MediaPlayList::*)(const QList<MusicAudioTrack> &,
                                                                         ElisaUtils::PlayListEnqueueMode,
                                                                         ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayList::enqueue));

#if defined KF5KIO_FOUND && KF5KIO_FOUND
    QObject::connect(d->mFileBrowserProxyModel.get(), &FileBrowserProxyModel::filesToEnqueue,
                     d->mMediaPlayList.get(), static_cast<void (MediaPlayList::*)(const QList<QUrl> &,
                                                                         ElisaUtils::PlayListEnqueueMode,
                                                                         ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayList::enqueue));
#endif
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
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::skipNextTrack, d->mMediaPlayList.get(), &MediaPlayList::skipNextTrack);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::sourceInError, d->mMediaPlayList.get(), &MediaPlayList::trackInError);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::sourceInError, d->mMusicManager.get(), &MusicListenersManager::playBackError);
    QObject::connect(d->mAudioControl.get(), &ManageAudioPlayer::playerSourceChanged, d->mAudioWrapper.get(), &AudioWrapper::setSource);

    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::ensurePlay, d->mAudioControl.get(), &ManageAudioPlayer::ensurePlay);
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::playListFinished, d->mAudioControl.get(), &ManageAudioPlayer::playListFinished);
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::currentTrackChanged, d->mAudioControl.get(), &ManageAudioPlayer::setCurrentTrack);

    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::playbackStateChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerPlaybackState);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::statusChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerStatus);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::errorChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerError);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::durationChanged, d->mAudioControl.get(), &ManageAudioPlayer::setAudioDuration);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::seekableChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerIsSeekable);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::positionChanged, d->mAudioControl.get(), &ManageAudioPlayer::setPlayerPosition);

    d->mPlayerControl->setPlayListModel(d->mMediaPlayList.get());
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::currentTrackChanged, d->mPlayerControl.get(), &ManageMediaPlayerControl::setCurrentTrack);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::playing, d->mPlayerControl.get(), &ManageMediaPlayerControl::playerPlaying);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::paused, d->mPlayerControl.get(), &ManageMediaPlayerControl::playerPaused);
    QObject::connect(d->mAudioWrapper.get(), &AudioWrapper::stopped, d->mPlayerControl.get(), &ManageMediaPlayerControl::playerStopped);

    d->mManageHeaderBar->setTitleRole(MediaPlayList::TitleRole);
    d->mManageHeaderBar->setAlbumRole(MediaPlayList::AlbumRole);
    d->mManageHeaderBar->setArtistRole(MediaPlayList::ArtistRole);
    d->mManageHeaderBar->setImageRole(MediaPlayList::ImageRole);
    d->mManageHeaderBar->setAlbumIdRole(MediaPlayList::AlbumIdRole);
    d->mManageHeaderBar->setIsValidRole(MediaPlayList::IsValidRole);
    d->mManageHeaderBar->setPlayListModel(d->mMediaPlayList.get());
    QObject::connect(d->mMediaPlayList.get(), &MediaPlayList::currentTrackChanged, d->mManageHeaderBar.get(), &ManageHeaderBar::setCurrentTrack);

    if (!d->mArguments.isEmpty()) {
        Q_EMIT enqueue(d->mArguments);
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

const QStringList &ElisaApplication::arguments() const
{
    return d->mArguments;
}

MusicListenersManager *ElisaApplication::musicManager() const
{
    return d->mMusicManager.get();
}

QSortFilterProxyModel *ElisaApplication::allAlbumsProxyModel() const
{
    return d->mAllAlbumsProxyModel.get();
}

QSortFilterProxyModel *ElisaApplication::allArtistsProxyModel() const
{
    return d->mAllArtistsProxyModel.get();
}

QSortFilterProxyModel *ElisaApplication::allGenresProxyModel() const
{
    return d->mAllGenresProxyModel.get();
}

QSortFilterProxyModel *ElisaApplication::allComposersProxyModel() const
{
    return d->mAllComposersProxyModel.get();
}

QSortFilterProxyModel *ElisaApplication::allLyricistsProxyModel() const
{
    return d->mAllLyricistsProxyModel.get();
}

QSortFilterProxyModel *ElisaApplication::allTracksProxyModel() const
{
    return d->mAllTracksProxyModel.get();
}

QSortFilterProxyModel *ElisaApplication::singleArtistProxyModel() const
{
    return d->mSingleArtistProxyModel.get();
}

QSortFilterProxyModel *ElisaApplication::singleAlbumProxyModel() const
{
    return d->mSingleAlbumProxyModel.get();
}

QSortFilterProxyModel *ElisaApplication::fileBrowserProxyModel() const
{
#if defined KF5KIO_FOUND && KF5KIO_FOUND
    return d->mFileBrowserProxyModel.get();
#else
    return nullptr;
#endif
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

#include "moc_elisaapplication.cpp"
