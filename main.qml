import QtQuick 2.15
import QtQuick.Window 2.15

Window {

    minimumHeight: 480
    maximumHeight: 480
    maximumWidth: 640
    minimumWidth: 640


    visible: true
    title: qsTr("Color Lines")

    GameUI {
        id: rootItem
        anchors.fill: parent
    }

}
