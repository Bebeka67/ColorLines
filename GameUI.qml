import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import Backend.Model 1.0

/*
  + 1.	Игровое поле должно представлять собой GridView либо TableView.
    2.	Логика поля должна находится в отдельном классе, наследуемом от QAbstractItemModel. Модель должна объявляться в qml файле

    3.
        Появление,
      + перемещение,
        удаление шариков должно быть анимированным.

        Анимации появления и исчезновения - плавное увеличение или уменьшение прозрачности соответственно.
        Анимация перемещения - плавное перемещение из одной позиции в другую (под словом “плавное” подразумевается,
        что должно быть визуально видно перемещение шарика из одной позиции в другую)

  + 5.	Полученные за время игры очки должны отображаться в цифровом виде в любой позиции окна,
        но не перекрывать собой игровое поле.
        Зачисление очков также должно быть анимированным (изменение числа со старого значения в новое)

    6.	Должна присутствовать кнопка “Новая игра”,
    при нажатии на которую игровое поле и счётчик очков будут очищены и начнётся новая игра

  + 7.	Перемещение шариков со старой позиции в новую осуществляется кликом по шарику и кликом в любую пустую клетку
    8.	Код должен быть понятным. Все неочевидные места должны сопровождаться комментариями. Приветствуется применение паттернов
*/

Control {
    id: rootItem

    background: Rectangle {
        color: "lightblue"
    }

    property int currentScore: 0

    RowLayout {
        anchors.fill: parent
        spacing: 1


        Item {
            id: gameField
            Layout.fillHeight: true
            Layout.preferredWidth: height


            TableView {
                id: tableView
                model: GameModel {id: gameModel}
                anchors.centerIn: parent
                width: contentWidth
                height: contentHeight
                interactive: false

                rowSpacing: 1
                columnSpacing: 1

                property int currentX: -1
                property int currentY: -1
                property color currentColor: "red"

                Rectangle {
                    id: dynamicIndicator

                    x: tableView.currentX
                    y: tableView.currentY
                    color: tableView.currentColor

                    height: 50 * 0.7
                    width: height
                    radius: height / 2
                    z: 10
                    visible: x && y !== -1

                    Behavior on x {
                        PropertyAnimation { duration: 500; easing.type: Easing.InOutBack }
                    }
                    Behavior on y {
                        PropertyAnimation { duration: 500; easing.type: Easing.InOutBack }
                    }
                }

                delegate: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 50

                    color: "white"
                    border.color: "black"

                    Text { text: parent.x + ":" + parent.y}
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            tableView.currentX = staticIndicator.mapToItem(tableView, 0, 0).x
                            tableView.currentY = staticIndicator.mapToItem(tableView, 0, 0).y
                            currentScore +=10
                        }
                    }

                    Rectangle {
                        id: staticIndicator

                        anchors.centerIn: parent
                        height: parent.height * 0.7
                        width: height
                        radius: height / 2

                        color: "transparent"
                    }
                }
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
                font.pixelSize: 30

                onTextChanged: PropertyAnimation {
                    target: gameScore
                    properties: "scale"
                    easing.type: Easing.InOutBack
                    duration: 500
                    easing.overshoot: 2
                    from: 0
                    to: scale
                }
            }

            Button {
                text: "New Game"
                width: parent.width
                anchors.bottom: parent.bottom
                onClicked: {
                    currentScore = 0
                    // gameModel.clear()
                }
            }

        }
    }









}
