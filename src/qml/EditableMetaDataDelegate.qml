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
    spacing: 0

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
        Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0
        Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0
    }

    Loader {
        id: textDisplayLoader

        focus: model.index === 0

        active: model.type === EditableTrackMetadataModel.TextEntry || model.type === EditableTrackMetadataModel.UrlEntry || model.type === EditableTrackMetadataModel.IntegerEntry
        visible: model.type === EditableTrackMetadataModel.TextEntry || model.type === EditableTrackMetadataModel.UrlEntry || model.type === EditableTrackMetadataModel.IntegerEntry

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        sourceComponent: TextField {
            text: model.display

            focus: model.index === 0

            horizontalAlignment: Text.AlignLeft

            selectByMouse: true

            anchors.fill: parent

            onTextEdited: {
                if (model.display !== text) {
                    model.display = text
                }
            }
        }
    }
}
