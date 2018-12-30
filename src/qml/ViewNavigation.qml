/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.7
import org.kde.elisa 1.0

ViewManager {
    property var currentStackView

    function openOneAlbum(stackView, albumTitle, albumAuthor, albumCover, albumDatabaseId)
    {
        currentStackView = stackView
        abstractOpenOneAlbum(albumTitle, albumAuthor, albumCover, albumDatabaseId)
    }

    function openOneArtist(stackView, artistName, artistImageUrl, artistDatabaseId)
    {
        currentStackView = stackView
        abstractOpenOneArtist(artistName, artistImageUrl, artistDatabaseId)
    }

    function openAllArtistsFromGenre(stackView, genreName)
    {
        currentStackView = stackView
        abstractOpenAllArtistsFromGenre(genreName)
    }

    function allAlbumsViewIsLoaded(stackView)
    {
        currentStackView = stackView
        abstractAllAlbumsViewIsLoaded()
    }

    function allArtistsViewIsLoaded(stackView)
    {
        currentStackView = stackView
        abstractAllArtistsViewIsLoaded()
    }

    function allGenresViewIsLoaded(stackView)
    {
        currentStackView = stackView
        abstractAllGenresViewIsLoaded()
    }

    function recentlyPlayedTracksIsLoaded(stackView)
    {
        currentStackView = stackView
        abstractRecentlyPlayedTracksIsLoaded()
    }

    function frequentlyPlayedTracksIsLoaded(stackView)
    {
        currentStackView = stackView
        abstractFrequentlyPlayedTracksIsLoaded()
    }

    onPopOneView: currentStackView.pop()
}
