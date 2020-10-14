/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

RowLayout {
    id: delegateRow

    property string name
    property int index
    property var type
    property string display

    spacing: 0

    signal edited()

    Label {
        id: metaDataLabels

        text: {
            if (name !== undefined) {
                return i18nc("Label for a piece of metadata, e.g. 'Album Artist:'", "%1:", name)
            }
            return ""
        }

        font.weight: Font.Bold

        horizontalAlignment: Text.AlignRight

        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: 0.8 * elisaTheme.coverImageSize
        Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0
        Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0
    }

    Loader {
        id: textDisplayLoader

        focus: index === 0

        active: type === EditableTrackMetadataModel.TextEntry || type === EditableTrackMetadataModel.UrlEntry || type === EditableTrackMetadataModel.IntegerEntry
        visible: type === EditableTrackMetadataModel.TextEntry || type === EditableTrackMetadataModel.UrlEntry || type === EditableTrackMetadataModel.IntegerEntry

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        sourceComponent: TextField {
            text: display

            focus: index === 0

            horizontalAlignment: Text.AlignLeft

            selectByMouse: true

            anchors.fill: parent

            onTextEdited: {
                if (display !== text) {
                    display = text

                    edited()
                }
            }
        }
    }

    Loader {
        focus: index === 0

        active: type === EditableTrackMetadataModel.RatingEntry
        visible: type === EditableTrackMetadataModel.RatingEntry

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        sourceComponent: RatingStar {
            starRating: display

            readOnly: false

            hoverWidgetOpacity: 1

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            onRatingEdited: {
                if (display !== starRating) {
                    display = starRating

                    edited()
                }
            }
        }
    }
}
