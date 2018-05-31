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
        NoViews,
        AllAlbums,
        OneAlbum,
        AllArtists,
        OneArtist,
        OneAlbumFromArtist,
        AllTracks
    }

    property int currentView: ViewManager.ViewsType.NoViews
    property string currentAlbumTitle
    property string currentAlbumAuthor
    property string currentArtistName
    property var currentStackView

    property int targetView: ViewManager.ViewsType.NoViews
    property string targetAlbumTitle
    property string targetAlbumAuthor
    property string targetArtistName
    property url targetImageUrl
    property int targetDatabaseId

    signal switchAllAlbumsView()
    signal switchOneAlbumView(var currentStackView, string mainTitle, url imageUrl, string secondaryTitle, int databaseId)
    signal switchAllArtistsView()
    signal switchOneArtistView(var currentStackView, string mainTitle, url imageUrl, string secondaryTitle, int databaseId)
    signal switchAllTracksView()
    signal switchOffAllViews()

    function closeAllViews()
    {
        currentView = ViewManager.ViewsType.NoViews
        targetView = ViewManager.ViewsType.NoViews
        switchOffAllViews()
    }

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

        if (currentView == ViewManager.ViewsType.AllAlbums) {
            targetView = ViewManager.ViewsType.OneAlbum
            switchOneAlbumView(currentStackView, targetAlbumTitle, targetImageUrl, targetAlbumAuthor, targetDatabaseId)
        } else if (currentView == ViewManager.ViewsType.OneArtist) {
            targetView = ViewManager.ViewsType.OneAlbumFromArtist
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

        if (currentView == ViewManager.ViewsType.AllArtists && targetView == ViewManager.ViewsType.OneArtist) {
            switchOneArtistView(currentStackView, targetArtistName, targetImageUrl, '', targetDatabaseId)
        } else if (currentView == ViewManager.ViewsType.OneArtist && currentArtistName != targetArtistName &&
                   targetView == ViewManager.ViewsType.OneArtist) {
            currentStackView.pop()
            switchOneArtistView(currentStackView, targetArtistName, targetImageUrl, '', targetDatabaseId)
        } else if (currentView == ViewManager.ViewsType.OneAlbumFromArtist && currentArtistName != targetArtistName &&
                   targetView == ViewManager.ViewsType.OneArtist) {
            currentStackView.pop()
            currentStackView.pop()
            switchOneArtistView(currentStackView, targetArtistName, targetImageUrl, '', targetDatabaseId)
        } else {
            switchAllArtistsView()
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
        } else if (targetView == ViewManager.ViewsType.OneAlbumFromArtist) {
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
        if (targetView == ViewManager.ViewsType.OneArtist) {
            currentView = ViewManager.ViewsType.OneArtist
        }
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
