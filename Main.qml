import QtQuick
import QtQuick.Controls
import NoteMama as NoteMama

Window {
    width: 480
    height: 480
    visible: true
    title: qsTr("Hello World")

    NoteMama.Canvas {
        id: canvas

        anchors.fill: parent

        sampleCount: 8

        Row {
            id: toolbar

            Button {
                text: "Raw"
                checked: canvas.inputMode == Canvas.Raw
                onClicked: canvas.inputMode = Canvas.Raw
            }

            Button {
                text: "Lines"
                checked: canvas.inputMode == Canvas.Lines
                onClicked: canvas.inputMode = Canvas.Lines
            }
        }

        Column {
            anchors.top: toolbar.bottom

            Text {
                text: "Drawn in " + (canvas.lastCompletedTime*1000).toFixed(5) + " μs"
            }
            Text {
                text: "Graphics api: " + canvas.graphicsApi
            }

            Text {
                text: "Width: " + canvas.width
            }
            Text {
                text: "Height: " + canvas.height
            }
        }
    }
}
