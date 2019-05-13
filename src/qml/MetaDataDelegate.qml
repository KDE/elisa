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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

import org.kde.elisa 1.0

RowLayout {
    id: delegateRow
    spacing: 0

    height: (model.type === TrackMetadataModel.LongTextEntry ? longTextDisplayLoader.height : (metaDataLabelMetric.boundingRect.height - metaDataLabelMetric.boundingRect.y)) +
            (elisaTheme.layoutVerticalMargin / 2)

    TextMetrics {
        id: metaDataLabelMetric

        text: 'Metadata Name'

        font.weight: Font.Bold
    }

    Label {
        id: metaDataLabels

        text: model.name

        font.weight: Font.Bold

        horizontalAlignment: Text.AlignLeft

        Layout.alignment: Qt.AlignTop
        Layout.preferredWidth: 0.8 * elisaTheme.coverImageSize
        Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin * 2 : 0
        Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin * 2 : 0
    }

    Loader {
        id: textDisplayLoader

        active: model.type === TrackMetadataModel.TextEntry || model.type === TrackMetadataModel.IntegerEntry
        visible: model.type === TrackMetadataModel.TextEntry || model.type === TrackMetadataModel.IntegerEntry

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        sourceComponent: LabelWithToolTip {
            text: model.display

            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight

            anchors.fill: parent
        }
    }

    Loader {
        id: longTextDisplayLoader

        active: model.type === TrackMetadataModel.LongTextEntry
        visible: model.type === TrackMetadataModel.LongTextEntry

        Layout.fillWidth: true
        Layout.maximumWidth: delegateRow.width - (0.8 * elisaTheme.coverImageSize + elisaTheme.layoutHorizontalMargin * 2)
        Layout.alignment: Qt.AlignTop

        sourceComponent: Label {
            text: model.display

            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight

            anchors.fill: parent

            wrapMode: Text.WordWrap
        }
    }

    Loader {
        active: model.type === TrackMetadataModel.DateEntry
        visible: model.type === TrackMetadataModel.DateEntry

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        sourceComponent: LabelWithToolTip {
            text: rawDate.toLocaleDateString()

            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight

            anchors.fill: parent

            property date rawDate: new Date(model.display)
        }
    }

    Loader {
        active: model.type === TrackMetadataModel.RatingEntry
        visible: model.type === TrackMetadataModel.RatingEntry

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        sourceComponent: RatingStar {
            starRating: model.display
            starSize: elisaTheme.ratingStarSize

            readOnly: true

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
        }
    }
}
