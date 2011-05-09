import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Rectangle {
    id: container
    // Parameters for editting
    property string text;
    property real itemX: 0
    property real itemY: 0
    property real itemZ: 0
    property variant camera: defaultCamera

    // Default values:
    property variant defaultCamera: Camera { eye: Qt.vector3d(0,5,30) }
    property real defaultWidth: 220
    property real defaultHeight: 150
    property real animationFactor: 0.0
    property real animationDuration: 2000

    SequentialAnimation on animationFactor {
        loops: Animation.Infinite
        PropertyAnimation {
            from: 0.0
            to: 1.0
            duration: animationDuration
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
        anchors.margins: 1
        anchors.topMargin: 2
        anchors.top: textItem.bottom
        anchors.bottom: container.bottom
        color: "#aaccee"

        Viewport {
            id: view
            anchors.fill: parent
            picking: true
            camera: container.camera
            Teapot
            {
                id: model
                x: itemX
                y: itemY
                z: itemZ
            }
        }
    }
}
