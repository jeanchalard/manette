// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 J <jean.chalard@gmail.com>

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Shapes
import QtMultimedia
import org.kde.kirigami as Kirigami
import org.j.manette

Kirigami.ApplicationWindow {
    id: root

    title: i18n("manette")

    minimumWidth: Kirigami.Units.gridUnit * 20
    minimumHeight: Kirigami.Units.gridUnit * 20

    onClosing: App.saveWindowGeometry(root)

    onWidthChanged: saveWindowGeometryTimer.restart()
    onHeightChanged: saveWindowGeometryTimer.restart()
    onXChanged: saveWindowGeometryTimer.restart()
    onYChanged: saveWindowGeometryTimer.restart()

    Component.onCompleted: App.restoreWindowGeometry(root)

    // This timer allows to batch update the window size change to reduce
    // the io load and also work around the fact that x/y/width/height are
    // changed when loading the page and overwrite the saved geometry from
    // the previous session.
    Timer {
        id: saveWindowGeometryTimer
        interval: 1000
        onTriggered: App.saveWindowGeometry(root)
    }

    EvdevController {
        id: controller

        property int top : 0
        property int left : 0
        property int bottom : 0
        property int right : 0
        onButtonStateChanged : (button, pressed) => {
//            console.log("button " + button + " is pressed : " + pressed)
            switch (button) {
                case X     : if (pressed) top    += 1; else top    -= 1; break
                case UP    : if (pressed) top    += 2; else top    -= 2; break
                case Y     : if (pressed) left   += 1; else left   -= 1; break
                case LEFT  : if (pressed) left   += 2; else left   -= 2; break
                case B     : if (pressed) bottom += 1; else bottom -= 1; break
                case DOWN  : if (pressed) bottom += 2; else bottom -= 2; break
                case A     : if (pressed) right  += 1; else right  -= 1; break
                case RIGHT : if (pressed) right  += 2; else right  -= 2; break
            }
            switch (button) {
                case X:
                case UP:
                    topShape.fillColor   = (top == 0) ? "#000000" : (top == 1) ? "#1128aa" : (top == 2) ? "#6f6f6f" : (top == 3) ? "#7585d5" : "#FFFFFF"
                    break
                case Y:
                case LEFT:
                    leftShape.fillColor  = (left == 0) ? "#000000" : (left == 1) ? "#298a45" : (left == 2) ? "#6f6f6f" : (left == 3) ? "#7fc492" : "#FFFFFF"
                    break
                case B:
                case DOWN:
                    bottomShape.fillColor  = (bottom == 0) ? "#000000" : (bottom == 1) ? "#dfd435" : (bottom == 2) ? "#6f6f6f" : (bottom == 3) ? "#efe994" : "#FFFFFF"
                    break
                case A:
                case RIGHT:
                    rightShape.fillColor = (right == 0) ? "#000000" : (right == 1) ? "#b50003" : (right == 2) ? "#6f6f6f" : (right == 3) ? "#da6d6e" : "#FFFFFF"
                    break
            }
            if (!pressed) return
            switch (button) {
                case B: downSound.stop(); downSound.play(); break
                case A: rightSound.stop(); rightSound.play(); break
                case X: topSound.stop(); topSound.play(); break
                case Y: leftSound.stop(); leftSound.play(); break
            }
        }
    }

    MediaPlayer {
        id : downSound
        audioOutput : AudioOutput {}
        source : "qrc:sounds/res/bottom.ogg"
    }
    MediaPlayer {
        id : leftSound
        audioOutput : AudioOutput {}
        source : "qrc:sounds/res/left.ogg"
    }
    MediaPlayer {
        id : rightSound
        audioOutput : AudioOutput {}
        source : "qrc:/sounds/res/right.ogg"
    }
    MediaPlayer {
        id : topSound
        audioOutput : AudioOutput {}
        source : "qrc:/sounds/res/top.ogg"
    }

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: !Kirigami.Settings.isMobile
        actions: [
            Kirigami.Action {
                text: i18n("About manette")
                icon.name: "help-about"
                onTriggered: root.pageStack.pushDialogLayer("qrc:/qt/qml/org/j/manette/contents/ui/About.qml")
            },
            Kirigami.Action {
                text: i18n("Quit")
                icon.name: "application-exit"
                onTriggered: Qt.quit()
            }
        ]
    }

    pageStack.initialPage: page

    Rectangle {
        id : page
        color : "black"
        border.width : 0
        width : 500
        height : 500

        function resize() {
            circleRadius = Math.min(width, height) / 2 - 20
        }

        property real circleRadius : Math.min(width, height) / 2
        property real cx : width / 2
        property real cy : height / 2
        onWidthChanged : resize()
        onHeightChanged : resize()

        Component.onCompleted : resize()

        Shape {
            anchors.fill : parent
            preferredRendererType : Shape.CurveRenderer
            ShapePath {
                id : topShape
                strokeWidth : 2
                strokeColor : "white"
                fillColor : "black"
                strokeStyle : ShapePath.SolidLine
                startX : page.cx
                startY : page.cy - 10
                PathLine {
                    x : page.cx - page.circleRadius * Math.sqrt(2) / 2
                    y : page.cy - page.circleRadius * Math.sqrt(2) / 2 - 10
                }
                PathArc {
                    x : page.cx + page.circleRadius * Math.sqrt(2) / 2
                    y : page.cy - page.circleRadius * Math.sqrt(2) / 2 - 10
                    radiusX : page.circleRadius ; radiusY : page.circleRadius
                }
                PathLine {
                    x : page.cx
                    y : page.cy - 10
                }
            }

            ShapePath {
                id : rightShape
                strokeWidth : 2
                strokeColor : "white"
                fillColor : "black"
                strokeStyle : ShapePath.SolidLine
                startX : page.cx + 10
                startY : page.cy
                PathLine {
                    x : page.cx + page.circleRadius * Math.sqrt(2) / 2 + 10
                    y : page.cy - page.circleRadius * Math.sqrt(2) / 2
                }
                PathArc {
                    x : page.cx + page.circleRadius * Math.sqrt(2) / 2 + 10
                    y : page.cy + page.circleRadius * Math.sqrt(2) / 2
                    radiusX : page.circleRadius ; radiusY : page.circleRadius
                }
                PathLine {
                    x : page.cx + 10
                    y : page.cy
                }
            }

            ShapePath {
                id : bottomShape
                strokeWidth : 2
                strokeColor : "white"
                fillColor : "black"
                strokeStyle : ShapePath.SolidLine
                startX : page.cx
                startY : page.cy + 10
                PathLine {
                    x : page.cx + page.circleRadius * Math.sqrt(2) / 2
                    y : page.cy + page.circleRadius * Math.sqrt(2) / 2 + 10
                }
                PathArc {
                    x : page.cx - page.circleRadius * Math.sqrt(2) / 2
                    y : page.cy + page.circleRadius * Math.sqrt(2) / 2 + 10
                    radiusX : page.circleRadius ; radiusY : page.circleRadius
                }
                PathLine {
                    x : page.cx
                    y : page.cy + 10
                }
            }

            ShapePath {
                id : leftShape
                strokeWidth : 2
                strokeColor : "white"
                fillColor : "black"
                strokeStyle : ShapePath.SolidLine
                startX : page.cx - 10
                startY : page.cy
                PathLine {
                    x : page.cx - page.circleRadius * Math.sqrt(2) / 2 - 10
                    y : page.cy + page.circleRadius * Math.sqrt(2) / 2
                }
                PathArc {
                    x : page.cx - page.circleRadius * Math.sqrt(2) / 2 - 10
                    y : page.cy - page.circleRadius * Math.sqrt(2) / 2
                    radiusX : page.circleRadius ; radiusY : page.circleRadius
                }
                PathLine {
                    x : page.cx - 10
                    y : page.cy
                }
            }

        }
    }
}
