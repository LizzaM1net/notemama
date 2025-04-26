import QtCore
import QtQml.Models
import QtQuick
import QtQuick.Controls
import NoteMama as NoteMama

Popup {
    id: toolSettingsPopup

    property var tool

    function openForTool(newTool) {
        tool = newTool
        toolSettings.model = tool.properties
        open()
    }

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    contentItem: ListView {
        id: toolSettings

        implicitWidth: 300
        implicitHeight: 400

        delegate: DelegateChooser {
            role: "type"
            DelegateChoice {
                roleValue: "Tool*"
                ItemDelegate {
                    width: toolSettings.implicitWidth
                    height: comboBox.height

                    text: "Tool: "

                    ComboBox {
                        id: comboBox
                        anchors.right: parent.right

                        textRole: "name"
                        model: canvas.tools

                        onCurrentValueChanged: tool[modelData.name] = currentValue
                    }
                }
            }
            DelegateChoice {
                roleValue: "bool"
                SwitchDelegate {
                    width: toolSettings.implicitWidth
                    text: modelData.name

                    onCheckedChanged: tool[modelData.name] = checked
                }
            }
        }
    }
}
