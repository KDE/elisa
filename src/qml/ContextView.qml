/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.10
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.2

import org.kde.elisa 1.0

FocusScope {
    id: topItem

    property int databaseId: 0
    property alias title: titleLabel.text
    property string albumName: ''
    property string artistName: ''
    property url albumArtUrl: ''
    property string fileUrl: ''

    TrackContextMetaDataModel {
        id: metaDataModel

        manager: elisa.musicManager
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        TextMetrics {
            id: titleHeight
            text: viewTitleHeight.text
            font
            {
                pointSize: viewTitleHeight.font.pointSize
                bold: viewTitleHeight.font.bold
            }
        }

        LabelWithToolTip {
            id: viewTitleHeight
            text: i18nc("Title of the context view related to the currently playing track", "Now Playing")

            font.pointSize: elisaTheme.defaultFontPointSize * 2

            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.topMargin: elisaTheme.layoutVerticalMargin * 3
            Layout.bottomMargin: titleHeight.height
        }

        Image {
            id: albumIcon

            source: albumArtUrl.toString() === '' ? Qt.resolvedUrl(elisaTheme.defaultAlbumImage) : albumArtUrl

            Layout.fillWidth: true
            Layout.maximumHeight: elisaTheme.contextCoverImageSize
            Layout.preferredHeight: elisaTheme.contextCoverImageSize

            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            width: elisaTheme.contextCoverImageSize
            height: elisaTheme.contextCoverImageSize

            sourceSize.width: parent.width
            sourceSize.height: elisaTheme.contextCoverImageSize

            asynchronous: true

            fillMode: Image.PreserveAspectCrop
        }

        LabelWithToolTip {
            id: titleLabel

            font.pointSize: elisaTheme.defaultFontPointSize * 2
            font.weight: Font.Bold

            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.topMargin: elisaTheme.layoutVerticalMargin

            elide: Text.ElideRight
        }

        LabelWithToolTip {
            id: albumArtistLabel

            text: (artistName && albumName ? i18nc('display of artist and album in context view', '<i>by</i> <b>%1</b> <i>from</i> <b>%2</b>', artistName, albumName) : '')

            font.pointSize: elisaTheme.defaultFontPointSize * 1.4

            visible: artistName !== '' && albumName !== ''

            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            elide: Text.ElideRight
        }

        LabelWithToolTip {
            id: albumLabel

            text: (albumName ? i18nc('display of album in context view', '<i>from</i> <b>%1</b>', albumName) : '')

            font.pointSize: elisaTheme.defaultFontPointSize * 1.4

            visible: artistName === '' && albumName !== ''

            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            elide: Text.ElideRight
        }

        LabelWithToolTip {
            id: artistLabel

            text: (artistName ? i18nc('display of artist in context view', '<i>by</i> <b>%1</b>', artistName) : '')

            font.pointSize: elisaTheme.defaultFontPointSize * 1.4

            visible: artistName !== '' && albumName === ''

            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            elide: Text.ElideRight
        }

        Repeater {
            model: metaDataModel

            delegate: MetaDataDelegate {
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Row {
            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            Layout.topMargin: elisaTheme.layoutVerticalMargin
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            spacing: elisaTheme.layoutHorizontalMargin

            Image {
                sourceSize.width: fileNameLabel.height
                sourceSize.height: fileNameLabel.height

                source: elisaTheme.folderIcon
            }

            LabelWithToolTip {
                id: fileNameLabel

                text: fileUrl

                elide: Text.ElideRight
            }
        }
    }

    onDatabaseIdChanged: {
        metaDataModel.initializeByTrackId(databaseId)
    }

    Connections {
        target: elisa

        onMusicManagerChanged: {
            metaDataModel.initializeByTrackId(databaseId)
        }
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            metaDataModel.initializeByTrackId(databaseId)
        }
    }
}
