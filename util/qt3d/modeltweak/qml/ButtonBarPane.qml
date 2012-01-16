import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0
import "fileHandling.js" as FileHandler
import "Widgets"

Flow {
    id: menu
    anchors.left: parent.left
    anchors.leftMargin: 2
    anchors.top: parent.top
    anchors.topMargin: 2
    width: buttonWidth * 4 + spacing * 3
    height: parent.height
    spacing: 5

    property double buttonWidth: 105

    SaveButton {
        id: save
        width: buttonWidth
        buttonText: "Save QML"
        imageSrc: "images/save.png"
    }

    BlenderToggle {
        id: load
        width: buttonWidth
        onClicked:
        {
            if (outerWindow.changed)
            {
                FileHandler.save_qml(true);
            }
            var useEffect = useCustomEffect;
            useCustomEffect = false;
            quickFile.load();
            useCustomEffect = useEffect;
        }
        buttonText: "Load Asset"
        imageSrc: "images/model.png"
    }

    BlenderToggle {
        id: help
        width: buttonWidth
        onClicked: helpOverlay.visible = true
        buttonText: "Help!"
        imageSrc: "images/help.png"
    }
}
