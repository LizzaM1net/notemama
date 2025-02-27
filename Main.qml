import QtQuick
import NoteMama as NoteMama

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    NoteMama.Canvas {
        anchors.fill: parent
    }
}
