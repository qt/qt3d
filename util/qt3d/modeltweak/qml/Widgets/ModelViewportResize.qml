import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0

Rectangle {
    radius: 4
    border.width: 1
    border.color: "#191919"
    width: 16 + 2
    height: 16 + 2
    color: "#999999"

    Image {
        id: img
        source: mainwindow.state !== "3Views" ? "images/shrink.png" : "images/grow.png"
        x: 2
        y: 2
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (view.stateName) {
                    if (mainwindow.state !== "3Views")
                        mainwindow.state = "3Views"
                    else
                        mainwindow.state = view.stateName
                }
            }
        }
    }
}
