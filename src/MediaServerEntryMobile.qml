import QtQuick 2.0
import QtQuick.Controls 1.2
import org.kde.kirigami 1.0 as MobileComponents

Item {
    property string imageName
    property string entryTitle

    signal clicked()

    MouseArea {
        id: clickHandle

        width: parent.width
        height: parent.height

        onClicked: {
            parent.clicked()
        }
    }

    Column {
        Image {
            id: mainIcon
            source: imageName
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit

            width: parent.width * 0.9
            height: parent.width * 0.9
        }
        Label {
            id: mainLabel
            text: entryTitle
            width: parent.width - 5
            elide: "ElideRight"
        }
    }
}

