import QtQuick 2.3
import QtQuick.Controls 1.2
import org.kde.plasma.mobilecomponents 0.2 as MobileComponents

MobileComponents.ApplicationWindow {
    visible: true
    title: qsTr("UPnP Player")

    MobileComponents.Label {
        text: qsTr("UPnP Player")
        anchors.centerIn: parent
    }

    globalDrawer: MobileComponents.GlobalDrawer {
        title: "UPnP Player"
    }
}

