import QtQuick 2.0
import org.mgallien.QmlExtension 1.0

Item {
    id: rootItem

    property alias playListModel: mpris2Interface.playListModel
    property alias playListControler: mpris2Interface.playListControler
    property alias audioPlayerManager: mpris2Interface.audioPlayerManager
    property alias headerBarManager: mpris2Interface.headerBarManager
    property alias manageMediaPlayerControl: mpris2Interface.manageMediaPlayerControl

    signal raisePlayer()

    Mpris2 {
        id: mpris2Interface

        playerName: 'elisa'

        onRaisePlayer:
        {
            rootItem.raisePlayer()
        }
    }
}
