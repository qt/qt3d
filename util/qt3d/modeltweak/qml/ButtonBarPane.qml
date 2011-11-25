import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0
import "Widgets"

Flow {
    width: buttonWidth * 4 + spacing * 3
    spacing: 8

    property double buttonWidth: 80

    SaveButton {
        width: buttonWidth
        buttonText: "Save!"
    }

    BlenderToggle {
        width: buttonWidth
        buttonText: "Load!"
        onClicked: quickFile.load()
    }

    BlenderToggle {
        width: buttonWidth
        buttonText: "Help!"
        onClicked: helpOverlay.visible = true
    }
}
