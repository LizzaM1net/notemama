import QtCore
import QtQml.Models
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

        PinchHandler {
            target: null

            onTranslationChanged: (delta) => {
                if (delta.x != 0) {
                    // print("translation", delta)
                    canvas.transformOrigin = delta
                }
            }
            // onRotationChanged: (delta) => print("rotation", delta)
            // onScaleChanged: (delta) => print("scale", delta)
            onScaleChanged: (delta) => canvas.scale *= delta
        }

        WheelHandler {
            target: null
            orientation: Qt.Horizontal | Qt.Vertical
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: (event) => canvas.move(event.pixelDelta)
        }

        Column {
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

            Text {
                text: "Position: " + canvas.position
            }
            Text {
                text: "Scale: " + canvas.scale
            }
            Text {
                text: "Transform origin: " + canvas.transformOrigin
            }
        }

        TextField {
            id: fileField
            anchors.right: parent.right
            Component.onCompleted: text = settings.file
        }

        ToolSettingsPopup {
            id: toolSettingsPopup
        }

        ListView {
            id: toolSelector

            anchors.bottom: parent.bottom
            width: parent.width
            height: 32

            orientation: ListView.Horizontal

            model: canvas.tools
            delegate: Button {
                text: modelData.name
                down: canvas.currentTool === modelData
                onClicked: {
                    if (!down) {
                        canvas.currentTool = modelData
                    } else {
                        toolSettingsPopup.openForTool(modelData)
                    }
                }
            }
        }
    }

    PdfParser {
        id: parser
        scene: canvas.scene
        file: fileField.text
    }

    Settings {
        id: settings
        property alias file: fileField.text
    }
}
