import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import Backend.Model 1.0

/*
  + 1.	Игровое поле должно представлять собой GridView либо TableView.
 +- 2.	Логика поля должна находится в отдельном классе, наследуемом от QAbstractItemModel. Модель должна объявляться в qml файле

    3.
      + Появление,
      + перемещение,
        удаление шариков должно быть анимированным.

        Анимации появления и исчезновения - плавное увеличение или уменьшение прозрачности соответственно.
        Анимация перемещения - плавное перемещение из одной позиции в другую (под словом “плавное” подразумевается,
        что должно быть визуально видно перемещение шарика из одной позиции в другую)

  + 5.	Полученные за время игры очки должны отображаться в цифровом виде в любой позиции окна,
        но не перекрывать собой игровое поле.
        Зачисление очков также должно быть анимированным (изменение числа со старого значения в новое)

  + 6.	Должна присутствовать кнопка “Новая игра”,
    при нажатии на которую игровое поле и счётчик очков будут очищены и начнётся новая игра

  + 7.	Перемещение шариков со старой позиции в новую осуществляется кликом по шарику и кликом в любую пустую клетку
    8.	Код должен быть понятным. Все неочевидные места должны сопровождаться комментариями. Приветствуется применение паттернов
*/

Control {
    id: rootItem

    background: Rectangle {
        color: "lightblue"
    }

    readonly property color blank: "#00000000"
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

                Item {
                    id: moveHandler

                    property bool moveMode: false

                    property point startCell
                    property point endCell

                    property color targetColor

                    signal clicked(int r, int c, point coords, color bColor, var button)

                    onClicked: {
                        if(button === Qt.RightButton) {
                            moveMode = false
                            startCell = Qt.point(-1,-1)
                            endCell = Qt.point(-1,-1)
                            targetColor = blank
                            console.log("cleared")
                            return
                        }

                        if(button === Qt.LeftButton) {
                            if(!moveMode && bColor === blank) {
                                console.log( "Blank cell" );
                                return
                            }

                            if(!moveMode && bColor !== blank) {
                                console.log("Colored Cell")
                                moveMode = true
                                startCell = Qt.point(r,c)
                                targetColor = bColor
                                dynamicIndicator.color = blank
                                dynamicIndicator.move(coords)
                                return
                            }

                            if(moveMode && bColor == blank) {
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
                            moveHandler.clicked(row, column, p, ballColor, mouse.button)
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

                        onColorChanged: {
                            color != blank ? appear.start() : hide.start()
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
                text: "New Game"
                width: parent.width
                anchors.bottom: parent.bottom
                onClicked: {
                    gameModel.clearField()
                    tableView.enabled = true
                    tableView.opacity = 1
                }
            }

        }
    }









}
