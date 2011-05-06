import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Rectangle {
    id: container
    property string text;
    property variant rotationAxis : Qt.vector3d(1,0,0)
    property real defaultWidth: 220
    property real defaultHeight: 150
    property variant rotation: Rotation3D {
        id: rotationTransform
        axis: rotationAxis
        PropertyAnimation on angle {
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 1500
        }
    }
    border.width: 2
    border.color: "black"
    radius: 5
    width: defaultWidth
    height: defaultHeight
    Text {
        id: textItem
        wrapMode: "WordWrap"
        horizontalAlignment: "AlignHCenter"
        text: container.text

        anchors.left: parent.left
        anchors.right: parent.right
    }
    Rectangle {
        id: viewportContainer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: textItem.bottom
        anchors.bottom: container.bottom
        color: "#aaccee"

        Viewport {
            id: view
            anchors.fill: parent
            picking: true
            camera: Camera { eye: Qt.vector3d(0,15,50)}
            Cube
            {
                id: model
                scale: 8.0
                transform: container.rotation
            }
        }
    }
}
