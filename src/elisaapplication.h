/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef ELISAAPPLICATION_H
#define ELISAAPPLICATION_H

#include "elisaLib_export.h"

#include "config-upnp-qt.h"

#include <QObject>
#include <QString>

#include <memory>

class QIcon;
class QAction;
class MusicListenersManager;
class AllAlbumsProxyModel;
class AllArtistsProxyModel;
class AllTracksProxyModel;
class SingleArtistProxyModel;
class SingleAlbumProxyModel;
class FileBrowserProxyModel;
class MediaPlayList;
class AudioWrapper;
class ManageAudioPlayer;
class ManageMediaPlayerControl;
class ManageHeaderBar;
class ElisaApplicationPrivate;

class ELISALIB_EXPORT ElisaApplication : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QStringList arguments
               READ arguments
               WRITE setArguments
               NOTIFY argumentsChanged)

    Q_PROPERTY(MusicListenersManager *musicManager
               READ musicManager
               NOTIFY musicManagerChanged)

    Q_PROPERTY(AllAlbumsProxyModel *allAlbumsProxyModel
               READ allAlbumsProxyModel
               NOTIFY allAlbumsProxyModelChanged)

    Q_PROPERTY(AllArtistsProxyModel *allArtistsProxyModel
               READ allArtistsProxyModel
               NOTIFY allArtistsProxyModelChanged)

    Q_PROPERTY(AllTracksProxyModel *allTracksProxyModel
               READ allTracksProxyModel
               NOTIFY allTracksProxyModelChanged)

    Q_PROPERTY(AllArtistsProxyModel *allGenresProxyModel
               READ allGenresProxyModel
               NOTIFY allGenresProxyModelChanged)

    Q_PROPERTY(AllArtistsProxyModel *allComposersProxyModel
               READ allComposersProxyModel
               NOTIFY allComposersProxyModelChanged)

    Q_PROPERTY(AllArtistsProxyModel *allLyricistsProxyModel
               READ allLyricistsProxyModel
               NOTIFY allLyricistsProxyModelChanged)

    Q_PROPERTY(SingleArtistProxyModel *singleArtistProxyModel
               READ singleArtistProxyModel
               NOTIFY singleArtistProxyModelChanged)

    Q_PROPERTY(SingleAlbumProxyModel *singleAlbumProxyModel
               READ singleAlbumProxyModel
               NOTIFY singleAlbumProxyModelChanged)

    Q_PROPERTY(FileBrowserProxyModel *fileBrowserProxyModel
               READ fileBrowserProxyModel
               NOTIFY fileBrowserProxyModelChanged)

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

public:
    explicit ElisaApplication(QObject *parent = nullptr);

    ~ElisaApplication() override;

    Q_INVOKABLE QAction* action(const QString& name);

    Q_INVOKABLE QString iconName(const QIcon& icon);

    const QStringList &arguments() const;

    MusicListenersManager *musicManager() const;

    AllAlbumsProxyModel *allAlbumsProxyModel() const;

    AllArtistsProxyModel *allArtistsProxyModel() const;

    AllArtistsProxyModel *allGenresProxyModel() const;

    AllArtistsProxyModel *allComposersProxyModel() const;

    AllArtistsProxyModel *allLyricistsProxyModel() const;

    AllTracksProxyModel *allTracksProxyModel() const;

    SingleArtistProxyModel *singleArtistProxyModel() const;

    SingleAlbumProxyModel *singleAlbumProxyModel() const;

    FileBrowserProxyModel *fileBrowserProxyModel() const;

    MediaPlayList *mediaPlayList() const;

    AudioWrapper *audioPlayer() const;

    ManageAudioPlayer *audioControl() const;

    ManageMediaPlayerControl *playerControl() const;

    ManageHeaderBar *manageHeaderBar() const;

Q_SIGNALS:

    void argumentsChanged();

    void musicManagerChanged();

    void allAlbumsProxyModelChanged();

    void allArtistsProxyModelChanged();

    void allGenresProxyModelChanged();

    void allComposersProxyModelChanged();

    void allLyricistsProxyModelChanged();

    void allTracksProxyModelChanged();

    void singleArtistProxyModelChanged();

    void singleAlbumProxyModelChanged();

    void fileBrowserProxyModelChanged();

    void mediaPlayListChanged();

    void audioPlayerChanged();

    void audioControlChanged();

    void playerControlChanged();

    void manageHeaderBarChanged();

    void enqueue(const QStringList &files);

public Q_SLOTS:

    void appHelpActivated();

    void aboutApplication();

    void reportBug();

    void configureShortcuts();

    void configureElisa();

    void setArguments(const QStringList &newArguments);

    void activateActionRequested(const QString &actionName, const QVariant &parameter);

    void activateRequested(const QStringList &arguments, const QString &workingDirectory);

    void openRequested(const QList< QUrl > &uris);

    void initialize();

private Q_SLOTS:

    void goBack();

    void find();

    void togglePlaylist();

private:

    void initializeModels();

    void initializePlayer();

    void setupActions(const QString &actionName);

    QStringList checkFileListAndMakeAbsolute(const QStringList &filesList, const QString &workingDirectory) const;

    std::unique_ptr<ElisaApplicationPrivate> d;

};

#endif // ELISAAPPLICATION_H
