import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Layouts 2

Pane {
    property alias model: entity_view.model

    signal entityClicked(int id)

    id: pane

    ColumnLayout {
        anchors.fill: parent

        Label {
            text: qsTr("Entités")
        }

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: entity_view

                width: parent.width
                height: parent.height

                delegate: ItemDelegate {
                    required property string name
                    required property string id

                    text: name

                    onClicked: pane.entityClicked(id)

                    ToolTip.delay: 2000
                    ToolTip.visible: hovered
                    ToolTip.text: "ID: " + id
                }
            }
        }
    }
}
