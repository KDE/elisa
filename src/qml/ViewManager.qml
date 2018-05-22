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
    property string currentAlbumTitle
    property string currentAlbumAuthor
    property string currentArtistName
    property var currentStackView

    property int targetView
    property string targetAlbumTitle
    property string targetAlbumAuthor
    property string targetArtistName
    property url targetImageUrl
    property int targetDatabaseId

    signal switchAllAlbumsView()
    signal switchOneAlbumView(var currentStackView, string mainTitle, string imageUrl, string secondaryTitle, int databaseId)
    signal switchAllArtistsView()
    signal switchOneArtistView(var currentStackView, string mainTitle, string imageUrl, string secondaryTitle, int databaseId)
    signal switchAllTracksView()

    function openAllAlbums()
    {
        targetView = ViewManager.ViewsType.AllAlbums

        if (currentView != targetView) {
            switchAllAlbumsView()
        }
    }

    function openOneAlbum(stackView, albumTitle, albumAuthor, albumCover, albumDatabaseId)
    {
        targetAlbumTitle = albumTitle
        targetAlbumAuthor = albumAuthor
        targetDatabaseId = albumDatabaseId
        targetImageUrl = albumCover
        currentStackView = stackView

        targetView = ViewManager.ViewsType.OneAlbum

        if (currentView == ViewManager.ViewsType.AllAlbums) {
            switchOneAlbumView(currentStackView, targetAlbumTitle, targetImageUrl, targetAlbumAuthor, targetDatabaseId)
        } else {
            switchAllAlbumsView()
        }
    }

    function openAllArtists()
    {
        targetView = ViewManager.ViewsType.AllArtists

        if (currentView != targetView) {
            switchAllArtistsView()
        }
    }

    function openOneArtist(stackView, artistName, artistImageUrl, artistDatabaseId)
    {
        targetArtistName = artistName
        targetDatabaseId = artistDatabaseId
        targetImageUrl = artistImageUrl
        currentStackView = stackView

        targetView = ViewManager.ViewsType.OneArtist

        if (currentView == ViewManager.ViewsType.AllArtists) {
            switchOneArtistView(currentStackView, targetArtistName, targetImageUrl, '', targetDatabaseId)
        } else if (currentView == ViewManager.ViewsType.OneArtist && currentArtistName != targetArtistName) {
            currentStackView.pop()
            switchOneArtistView(currentStackView, targetArtistName, targetImageUrl, '', targetDatabaseId)
        } else if (currentView == ViewManager.ViewsType.OneAlbumFromArtist && currentArtistName != targetArtistName) {
            currentStackView.pop()
            currentStackView.pop()
            switchOneArtistView(currentStackView, targetArtistName, targetImageUrl, '', targetDatabaseId)
        } else {
            switchAllArtistsView()
        }
    }

    function openOneAlbumFromArtist(stackView, albumTitle, albumAuthor, albumCover, albumDatabaseId)
    {
        targetAlbumTitle = albumTitle
        targetAlbumAuthor = albumAuthor
        targetDatabaseId = albumDatabaseId
        targetImageUrl = albumCover
        currentStackView = stackView

        targetView = ViewManager.ViewsType.OneAlbumFromArtist

        if (currentView == ViewManager.ViewsType.OneArtist) {
            switchOneAlbumView(currentStackView, targetAlbumTitle, targetImageUrl, targetAlbumAuthor, targetDatabaseId)
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

    function allAlbumsViewIsLoaded(stackView)
    {
        currentStackView = stackView
        currentView = ViewManager.ViewsType.AllAlbums
        if (targetView == ViewManager.ViewsType.OneAlbum) {
            switchOneAlbumView(currentStackView, targetAlbumTitle, targetImageUrl, targetArtistName, targetDatabaseId)
        }
    }

    function oneAlbumViewIsLoaded()
    {
        currentAlbumTitle = targetAlbumTitle
        currentAlbumAuthor = targetAlbumAuthor

        if (targetView == ViewManager.ViewsType.OneAlbum) {
            currentView = ViewManager.ViewsType.OneAlbum
        } else {
            currentView = ViewManager.ViewsType.OneAlbumFromArtist
        }
    }

    function allArtistsViewIsLoaded(stackView)
    {
        currentStackView = stackView
        currentView = ViewManager.ViewsType.AllArtists
        if (targetView == ViewManager.ViewsType.OneArtist) {
            switchOneArtistView(currentStackView, targetArtistName, targetImageUrl, '', targetDatabaseId)
        }
    }

    function oneArtistViewIsLoaded()
    {
        currentArtistName = targetArtistName
        currentView = ViewManager.ViewsType.OneArtist
    }

    function allTracksViewIsLoaded(allTracksView)
    {
        currentView = ViewManager.ViewsType.AllTracks
    }

    function goBack()
    {
        currentStackView.pop()

        if (currentView == ViewManager.ViewsType.OneAlbum) {
            currentView = ViewManager.ViewsType.AllAlbums
        } else if (currentView == ViewManager.ViewsType.OneArtist) {
            currentView = ViewManager.ViewsType.AllArtists
        } else if (currentView == ViewManager.ViewsType.OneAlbumFromArtist) {
            currentView = ViewManager.ViewsType.OneArtist
        }
    }
}
