import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0
import "Widgets"
import "fileHandling.js" as FileHandler


BlenderToggle {
    onClicked: {
        FileHandler.save_qml(false);
    }
}
