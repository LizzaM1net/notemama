import QtQuick
import NoteMama as NoteMama

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    NoteMama.Canvas {
        id: canvas

        anchors.fill: parent
    }

    Text {
        text: "Drawn in " + (canvas.lastCompletedTime*1000).toFixed(5) + " μs"
    }
}
