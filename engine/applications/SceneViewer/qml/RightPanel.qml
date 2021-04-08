import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Layouts 2

import StormKit.Scene 1.0
import StormKit.Component.Reflector 1.0

Pane {
    property StormKitScene scene

    id: pane

    function introspect(id) {
        console.log(reflector.hasComponent(ComponentReflector.ComponentType.Name));
    }

    ComponentReflector {
        id: reflector

        scene: scene
    }

    ColumnLayout {
        id: layout

        anchors.fill: parent

        Label {
            text: qsTr("Composants")
        }

        Frame {
            id: component_frame

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
