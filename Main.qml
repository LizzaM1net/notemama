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

        // sampleCount: 8

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
                color: "white"
                text: "Drawn in " + (canvas.lastCompletedTime*1000).toFixed(5) + " μs"
            }
            Text {
                color: "white"
                text: "Graphics api: " + canvas.graphicsApi
            }

            Text {
                color: "white"
                text: "Width: " + canvas.width
            }
            Text {
                color: "white"
                text: "Height: " + canvas.height
            }

            Text {
                color: "white"
                text: "Position: " + canvas.position
            }
            Text {
                color: "white"
                text: "Scale: " + canvas.scale
            }
            Text {
                color: "white"
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

        RoundButton {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            icon.name: "network-wireless"

            onClicked: hotspotPopup.open()

            Popup {
                id: hotspotPopup

                parent: canvas

                anchors.centerIn: parent
                width: parent.width/2
                height: parent.height/2

                modal: true
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                Column {
                    SwitchDelegate {
                        id: serverSwitch
                        text: "Server"
                    }

                    ItemDelegate {
                        text: server.port
                        visible: server.port
                    }

                    TextField {
                        id: urlField
                    }

                    ItemDelegate {
                        text: client.connected ? "connected" : "disconnected"
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

    NoteMama.RemoteServer {
        id: server
        canvas: canvas
        enabled: serverSwitch.checked
    }

    NoteMama.RemoteClient {
        id: client
        canvas: canvas
        url: urlField.text
    }

    Settings {
        id: settings
        property alias file: fileField.text
    }
}
