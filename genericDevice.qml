import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    property var aDevice
    property StackView stackView

    Button {
        id: backButton
        height: 50
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        onClicked: stackView.pop()
        text: 'Back'
    }

    Label {
        anchors.top: backButton.bottom
        text: 'hello'
    }
}

