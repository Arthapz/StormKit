import QtQuick 2
import QtQuick.Window 2
import QtQuick.Controls 2
import QtQuick.Layouts 2


ApplicationWindow {
    id: main_window
    objectName: "main_window"
    visible: true
    title: qsTr("Hello World")

    Text{ id: default_font }

    font.pointSize: default_font.font.pointSize + 2

    palette.window: "#353535"
    palette.windowText: "white"
    //disabled 127, 127, 127
    palette.base: "#2a2a2a"
    palette.alternateBase: "#424242"
    palette.toolTipBase: "white"
    palette.toolTipText: "#353535"
    palette.text: "white"
    palette.dark: "#232323"
    palette.shadow: "#141414"
    palette.button: "#353535"
    palette.buttonText: "white"
    palette.brightText: "red"
    palette.link: "#2a82da"
    palette.highlight: "#2a82da"
    palette.highlightedText: "white"

    width: 1280
    height: 720

    Action {
        id: toggle_entities_panel
        text: qsTr("Afficher les entités")

        checkable: true
        checked: true
        onToggled: show(checked)

        function show(enabled) {
            entities_panel.visible = enabled
        }
    }

    Action {
        id: toggle_components_panel
        text: qsTr("Afficher les composants")

        checkable: true
        checked: false
        onToggled: show(checked)

        function show(enabled) {
            components_panel.visible = enabled
        }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: show_entities

                text: qsTr("Entités")
                checkable: true
                checked: true

                action: toggle_entities_panel
            }

            ToolButton {
                id: show_components

                text: qsTr("Composants")
                checkable: true
                checked: false

                action: toggle_components_panel
            }

            Pane {
                Layout.fillWidth: true
            }
        }
    }

    RowLayout {
        id: grid

        anchors.fill: parent

        LeftPanel {
            id: entities_panel

            Layout.fillHeight: true
            Layout.minimumWidth: 150

            onEntityClicked: {
                show_components.checked = true;
                toggle_components_panel.show(true)
                components_panel.introspect(id);
            }
        }

        MainPanel {
            id: main_panel

            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        RightPanel {
            id: components_panel

            visible: false

            Layout.fillHeight: true
            Layout.minimumWidth: 400

            scene: main_panel.scene
        }
    }
}
