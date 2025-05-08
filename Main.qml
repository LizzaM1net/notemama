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
                if (delta.x != 0)
                    canvas.transformOrigin = delta
            }
            onScaleChanged: (delta) => canvas.scale *= delta
        }

        WheelHandler {
            target: null
            orientation: Qt.Horizontal | Qt.Vertical
            acceptedDevices: PointerDevice.TouchPad
            onWheel: (event) => canvas.move(event.pixelDelta)
        }

        WheelHandler {
            target: null
            orientation: Qt.Vertical
            acceptedDevices: PointerDevice.Mouse
            onWheel: (event) => {
                canvas.transformOrigin = point.position
                canvas.scale*=(1.4**(event.angleDelta.y/360))
            }
        }

        DragHandler {
            target: null
            acceptedButtons: Qt.MiddleButton
            acceptedDevices: PointerDevice.Mouse
            property vector2d startPosition
            onActiveChanged: {
                if (active)
                    startPosition = canvas.position
            }
            onActiveTranslationChanged: canvas.position = startPosition.minus(activeTranslation.times(1/canvas.scale))
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

    NoteMama.PdfParser {
        id: parser
        scene: canvas.scene
        file: fileField.text
    }


    Settings {
        id: settings
        property alias file: fileField.text
    }
}
