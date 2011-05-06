import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Rectangle {
    id: container
    // Parameters for editting
    property string text;
    property variant testTransform: defaultTransform
    property real scaleFactor: 1.0
    property variant scaleVectorA: Qt.vector3d(1,1,1)
    property variant scaleVectorB: Qt.vector3d(1,1,1)
    property variant camera: defaultCamera

    // Default values:
    property variant defaultCamera: Camera { eye: Qt.vector3d(0,5,5)}
    property real defaultWidth: 220
    property real defaultHeight: 150
    property variant defaultTransform: Scale3D {
        scale: Qt.vector3d((scaleVectorA.x * animationFactor +
                            scaleVectorB.x * (1.0 - animationFactor)),
                           scaleVectorA.y * animationFactor +
                           scaleVectorB.y * (1.0 - animationFactor),
                           scaleVectorA.z * animationFactor +
                           scaleVectorB.z * (1.0 - animationFactor))
        property real animationFactor: 1.0

        SequentialAnimation on animationFactor {
            loops: Animation.Infinite
            PropertyAnimation {
                from: 1.0
                to: 0.0
                duration: 2000
            }
            PropertyAnimation {
                from: 0.0
                to: 1.0
                duration: 2000
            }
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
                scale: container.scaleFactor
                transform: container.testTransform
            }
        }
    }
}
