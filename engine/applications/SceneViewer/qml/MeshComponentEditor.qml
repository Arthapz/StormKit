import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Layouts 2

import StormKit.Scene 1.0
import StormKit.Component.Model 1.0

Frame {
    property string component_data

    id: pane

    RowLayout {
        anchors.fill: parent

        Label {
            text: "MeshComponent"
        }
    }
}
