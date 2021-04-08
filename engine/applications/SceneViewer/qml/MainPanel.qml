import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Controls.Material 2
import QtQuick.Layouts 2

import StormKit.View 1.0

StormKitView {
    id: engine_view

    onInitialized: {
        components_panel.scene = engine_view.scene
        entities_panel.model = engine_view.entities
    }
}
