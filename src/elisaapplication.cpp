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
#include "mediaplaylist.h"
#include "audiowrapper.h"

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
    }

#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    KActionCollection mCollection;
#endif

    QStringList mArguments;

    std::unique_ptr<MusicListenersManager> mMusicManager;

    std::unique_ptr<AllAlbumsProxyModel> mAllAlbumsProxyModel;

    std::unique_ptr<AllArtistsProxyModel> mAllArtistsProxyModel;

    std::unique_ptr<AllTracksProxyModel> mAllTracksProxyModel;

    std::unique_ptr<SingleArtistProxyModel> mSingleArtistProxyModel;

    std::unique_ptr<SingleAlbumProxyModel> mSingleAlbumProxyModel;

    std::unique_ptr<MediaPlayList> mMediaPlayList;

    std::unique_ptr<AudioWrapper> mAudioWrapper;

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
    d->mMusicManager = std::make_unique<MusicListenersManager>();
    Q_EMIT musicManagerChanged();
    d->mAllAlbumsProxyModel = std::make_unique<AllAlbumsProxyModel>();
    Q_EMIT allAlbumsProxyModelChanged();
    d->mAllArtistsProxyModel = std::make_unique<AllArtistsProxyModel>();
    Q_EMIT allArtistsProxyModelChanged();
    d->mAllTracksProxyModel = std::make_unique<AllTracksProxyModel>();
    Q_EMIT allTracksProxyModelChanged();
    d->mSingleArtistProxyModel = std::make_unique<SingleArtistProxyModel>();
    Q_EMIT singleArtistProxyModelChanged();
    d->mSingleAlbumProxyModel = std::make_unique<SingleAlbumProxyModel>();
    Q_EMIT singleAlbumProxyModelChanged();
    d->mMediaPlayList = std::make_unique<MediaPlayList>();
    Q_EMIT mediaPlayListChanged();
    d->mAudioWrapper = std::make_unique<AudioWrapper>();
    Q_EMIT audioPlayerChanged();

    d->mMusicManager->setElisaApplication(this);

    d->mMediaPlayList->setMusicListenersManager(d->mMusicManager.get());
    QObject::connect(this, &ElisaApplication::enqueue, d->mMediaPlayList.get(), &MediaPlayList::enqueueAndPlay);

    d->mAllAlbumsProxyModel->setSourceModel(d->mMusicManager->allAlbumsModel());
    d->mAllArtistsProxyModel->setSourceModel(d->mMusicManager->allArtistsModel());
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

AllAlbumsProxyModel *ElisaApplication::allAlbumsProxyModel() const
{
    return d->mAllAlbumsProxyModel.get();
}

AllArtistsProxyModel *ElisaApplication::allArtistsProxyModel() const
{
    return d->mAllArtistsProxyModel.get();
}

AllTracksProxyModel *ElisaApplication::allTracksProxyModel() const
{
    return d->mAllTracksProxyModel.get();
}

SingleArtistProxyModel *ElisaApplication::singleArtistProxyModel() const
{
    return d->mSingleArtistProxyModel.get();
}

SingleAlbumProxyModel *ElisaApplication::singleAlbumProxyModel() const
{
    return d->mSingleAlbumProxyModel.get();
}

MediaPlayList *ElisaApplication::mediaPlayList() const
{
    return d->mMediaPlayList.get();
}

AudioWrapper *ElisaApplication::audioPlayer() const
{
    return d->mAudioWrapper.get();
}

#include "moc_elisaapplication.cpp"
