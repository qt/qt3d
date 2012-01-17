import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0

Rectangle {
    radius: 4
    border.width: 1
    border.color: "#191919"
    width: 16 + 2
    height: 16 + 2 + 16 + 2
    color: "#999999"

    signal zoomIn()
    signal zoomOut()

    Image {
        id: imgIn
        source: "images/zoomin.png"
        x: 2
        y: 2
        MouseArea {
            anchors.fill: parent
            onClicked: {
                zoomIn()
            }
        }
    }

    Image {
        id: imgOut
        source: "images/zoomout.png"
        x: 2
        y: 20
        MouseArea {
            anchors.fill: parent
            onClicked: {
                zoomOut()
            }
        }
    }
}
