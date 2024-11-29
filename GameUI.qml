import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import Backend.Model 1.0

Control {
    id: rootItem

    background: Rectangle {
        color: "lightblue"
    }

    property int currentScore: gameModel.score

    GameModel {
        id: gameModel
        onGameOver: {
            console.log("game over")
            tableView.enabled = false
            tableView.opacity = 0.7
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 1

        Item {
            id: gameField
            Layout.fillHeight: true
            Layout.preferredWidth: height

            TableView {
                id: tableView
                model: gameModel
                anchors.centerIn: parent
                width: contentWidth
                height: contentHeight
                interactive: false

                rowSpacing: 1
                columnSpacing: 1

                Rectangle {
                    id: dynamicIndicator

                    function move(point) {
                        this.x = point.x
                        this.y = point.y
                    }

                    color: "transparent"
                    height: 50 * 0.7
                    width: height
                    radius: height / 2
                    z: 10

                    Behavior on x {
                        PropertyAnimation {
                            duration: 500;
                            easing.type: Easing.InOutBack;
                            onRunningChanged:  running ? "" : dynamicIndicator.color = "transparent"
                        }
                    }

                    Behavior on y {
                        PropertyAnimation {
                            duration: 500;
                            easing.type: Easing.InOutBack;
                            onRunningChanged:  running ? "" : dynamicIndicator.color = "transparent"
                        }
                    }
                }

                Rectangle {
                    id: selectedIndicator

                    z: 10
                    width: 50
                    height: 50

                    color: "transparent"
                    border.color: Qt.lighter(moveHandler.targetColor, 1.3)
                    border.width: 3
                    radius: 4
                    x: moveHandler.startCoords.x - ((width * 0.7) / 4) + 2
                    y: moveHandler.startCoords.y - ((height * 0.7) / 4) + 2
                    visible: moveHandler.moveMode

                }
                Item {
                    id: moveHandler

                    property bool moveMode: false

                    property point startCell
                    property point endCell

                    property point startCoords

                    property color targetColor

                    signal clicked(int r, int c, point coords, color bColor, bool cellIsEmpty, var button)

                    onClicked: {
                        if(button === Qt.RightButton) {
                            moveMode = false
                            startCell = Qt.point(-1,-1)
                            endCell = Qt.point(-1,-1)
                            console.log("cleared")
                            return
                        }

                        if(button === Qt.LeftButton) {
                            if(!moveMode && cellIsEmpty) {
                                console.log( "Blank cell" );
                                return
                            }

                            if(!moveMode && !cellIsEmpty) {
                                console.log("Colored Cell: " + bColor)
                                moveMode = true
                                startCell = Qt.point(r,c)
                                startCoords = coords
                                targetColor = bColor
                                dynamicIndicator.move(coords)
                                return
                            }

                            if(moveMode && cellIsEmpty) {
                                endCell = Qt.point(r,c)
                                dynamicIndicator.color = targetColor
                                dynamicIndicator.move(coords)
                                console.log("moved")
                                gameModel.move(startCell, endCell)
                                moveMode = false
                                return
                            }
                        }
                    }
                }

                delegate: Control {
                    implicitWidth: 50
                    implicitHeight: 50


                    background: Rectangle {
                        radius: 2
                        color: "white"
                        border.color: "black"
                        opacity: 0.7
                    }

                    MouseArea {
                        anchors.fill: parent

                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onClicked: {
                            let p = Qt.point(staticIndicator.mapToItem(tableView, 0, 0).x,
                                             staticIndicator.mapToItem(tableView, 0, 0).y)
                            moveHandler.clicked(row, column, p, ballColor, isEmpty, mouse.button)
                        }
                    }

                    Rectangle {
                        id: staticIndicator

                        anchors.centerIn: parent
                        height: parent.height * 0.7
                        width: height
                        radius: height / 2

                        color: ballColor
                        border.color: Qt.darker(color, 2)

                        opacity: 1

                        property bool empty: isEmpty

                        onEmptyChanged: {
                            empty ? hide.start() : appear.start()
                        }

                        PropertyAnimation {
                            id: appear
                            target: staticIndicator;
                            property: "opacity"
                            from: 0
                            to: 1
                            duration: 1000
                        }
                        PropertyAnimation {
                            id: hide
                            target: staticIndicator;
                            property: "opacity"
                            from: 1
                            to: 0
                            duration: 1000
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                font.pixelSize: 30

                horizontalAlignment: Qt.AlignHCenter
                text: "Game Over\nTotal Score: %0".arg(currentScore);

                background: Rectangle {
                    anchors.fill: parent
                    anchors.margins: - 10
                    radius: 3
                    opacity: 0.9
                    border.color: "lightgray"
                }

                visible: !tableView.enabled
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 1
            color: "gray"
        }

        Item {
            id: sideBar

            Layout.fillHeight: true
            Layout.fillWidth: true

            Label {
                id: rools
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                font.pixelSize: 20
                horizontalAlignment: Qt.AlignHCenter
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: "Правила:\nЛевый клик - выбрать шарик\nПравый клик - отменить выбор"
            }

            Label {
                id: gameScore
                anchors.centerIn: parent
                text: currentScore
                font.pixelSize: 60

                onTextChanged: PropertyAnimation {
                    target: gameScore
                    properties: "scale"
                    easing.type: Easing.InOutBack
                    duration: 1000
                    easing.overshoot: 3
                    from: 0.5
                    to: scale
                }
            }

            Button {
                id: newGameButton

                Text {
                    anchors.centerIn: parent
                    horizontalAlignment: Qt.AlignHCenter
                    text: "Новая игра"
                    font.pixelSize: 20
                }

                width: parent.width

                height: 30

                background: Rectangle {
                    color: palette.button
                    radius: 3
                }
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.margins: 10;

                onClicked: {
                    gameModel.clearField()
                    tableView.enabled = true
                    tableView.opacity = 1
                }
            }

        }
    }

}
