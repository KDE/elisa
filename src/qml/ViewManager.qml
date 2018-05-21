/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.7

QtObject {

    enum ViewsType {
        AllAlbums,
        OneAlbum,
        AllArtists,
        OneArtist,
        OneAlbumFromArtist,
        AllTracks
    }

    property int currentView
    property int targetView

    property string targetAlbumTitle
    property string targetArtistName
    property url targetImageUrl
    property int targetDatabaseId
    property var targetStackView

    signal switchAllAlbumsView()
    signal switchOneAlbumView(var currentStackView, string mainTitle, string imageUrl, string secondaryTitle, int databaseId)
    signal switchAllArtistsView()
    signal switchOneArtistView(var currentStackView, string mainTitle, string imageUrl, string secondaryTitle, int databaseId)
    signal switchAllTracksView()

    function openAllAlbums()
    {
        console.log('open all albums')

        targetView = ViewManager.ViewsType.AllAlbums

        console.log('target view is ' + targetView)

        if (currentView != targetView) {
            switchAllAlbumsView()
        }
    }

    function openOneAlbum(currentStackView, albumTitle, albumAuthor, albumCover, albumDatabaseId)
    {
        console.log('open album ' + albumTitle + ' from ' + albumAuthor)

        targetAlbumTitle = albumTitle
        targetArtistName = albumAuthor
        targetDatabaseId = albumDatabaseId
        targetImageUrl = albumCover
        targetStackView = currentStackView

        targetView = ViewManager.ViewsType.OneAlbum

        console.log('target view is ' + targetView)

        if (currentView == ViewManager.ViewsType.AllAlbums) {
            switchOneAlbumView(targetStackView, targetAlbumTitle, targetImageUrl, targetArtistName, targetDatabaseId)
        } else {
            switchAllAlbumsView()
        }
    }

    function openAllArtists()
    {
        console.log('open all artists')

        targetView = ViewManager.ViewsType.AllArtists

        console.log('target view is ' + targetView)

        if (currentView != targetView) {
            switchAllArtistsView()
        }
    }

    function openOneArtist(currentStackView, artistName, artistImageUrl, artistDatabaseId)
    {
        console.log('open artist ' + artistName)
        targetArtistName = artistName
        targetDatabaseId = artistDatabaseId
        targetImageUrl = artistImageUrl
        targetStackView = currentStackView

        targetView = ViewManager.ViewsType.OneArtist

        console.log('target view is ' + targetView)

        if (currentView == ViewManager.ViewsType.AllArtists) {
            switchOneArtistView(targetStackView, targetArtistName, targetImageUrl, '', targetDatabaseId)
        } else {
            switchAllArtistsView()
        }
    }

    function openOneAlbumFromArtist(currentStackView, albumTitle, albumAuthor, albumCover, albumDatabaseId)
    {
        targetAlbumTitle = albumTitle
        targetArtistName = albumAuthor
        targetDatabaseId = albumDatabaseId
        targetImageUrl = albumCover
        targetStackView = currentStackView

        targetView = ViewManager.ViewsType.OneAlbumFromArtist

        if (currentView == ViewManager.ViewsType.AllArtists) {
            switchOneAlbumView(targetStackView, targetAlbumTitle, targetImageUrl, targetArtistName, targetDatabaseId)
        } else {
            switchAllAlbumsView()
        }
    }

    function openAllTracks()
    {
        targetView = ViewManager.ViewsType.AllTracks
        if (currentView != targetView) {
            switchAllTracksView()
        }
    }

    function allAlbumsViewIsLoaded(currentStackView)
    {
        console.log('all albums view is loaded and target view is ' + targetView)
        targetStackView = currentStackView
        currentView = ViewManager.ViewsType.AllAlbums
        if (targetView == ViewManager.ViewsType.OneAlbum) {
            switchOneAlbumView(targetStackView, targetAlbumTitle, targetImageUrl, targetArtistName, targetDatabaseId)
        }
    }

    function oneAlbumViewIsLoaded(oneAlbumView)
    {
        currentView = ViewManager.ViewsType.OneAlbum
    }

    function allArtistsViewIsLoaded(currentStackView)
    {
        console.log('all artists view is loaded and target view is ' + targetView)
        targetStackView = currentStackView
        currentView = ViewManager.ViewsType.AllArtists
        if (targetView == ViewManager.ViewsType.OneArtist) {
            console.log('switch to artist: ' + targetArtistName)
            switchOneArtistView(targetStackView, targetArtistName, targetImageUrl, '', targetDatabaseId)
        }
    }

    function oneArtistViewIsLoaded(oneArtistView)
    {
        currentView = ViewManager.ViewsType.OneArtist
    }

    function allTracksViewIsLoaded(allTracksView)
    {
        currentView = ViewManager.ViewsType.AllTracks
    }
}
