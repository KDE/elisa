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

    signal radioEdited()

    TextMetrics {
        id: metaDataLabelMetric

        text: 'Metadata Name'
    }

    Label {
        id: metaDataLabels

        text: {
            if (model.name !== undefined) {
                return i18nc("Label for a piece of metadata, e.g. 'Album Artist:'", "%1:", model.name)
            }
            return ""
        }

        font.weight: Font.Bold

        horizontalAlignment: Text.AlignRight

        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: 0.8 * elisaTheme.coverImageSize
        Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin / 2 : 0
        Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin / 2 : 0
    }

    Loader {
        id: textDisplayLoader

        active: model.type === TrackMetadataModel.TextEntry || model.type === TrackMetadataModel.IntegerEntry
        visible: model.type === TrackMetadataModel.TextEntry || model.type === TrackMetadataModel.IntegerEntry

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        sourceComponent: TextField {
            text: model.display

            horizontalAlignment: Text.AlignLeft

            selectByMouse: true

            anchors.fill: parent

            onTextEdited: {
                if (model.display !== text) {
                    model.display = text
                    delegateRow.radioEdited()
                }
            }
        }
    }
}
