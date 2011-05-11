import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Rectangle {
    id: container
    property string text:
        "This example combines a rotation, scale and translate in one " +
        "Item3D.  It should show a teapot spinning anti-clockwise, growing " +
        "and moving up-right relative to the initial camera, and then " +
        "reversing.";
    property variant camera: defaultCamera

    // Default values:
    property variant defaultCamera: Camera { eye: Qt.vector3d(0,5,30) }
    property real defaultWidth: 440
    property real defaultHeight: 300
    property real animationFactor: 0.0
    property real animationDuration: 2000

    SequentialAnimation on animationFactor {
        loops: Animation.Infinite
        PropertyAnimation {
            from: 0.0
            to: 1.0
            duration: animationDuration
        }
        PropertyAnimation {
            from: 1.0
            to: 0.0
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
                id: testModel
                property real scaleX: 1 + (2.0 * animationFactor);
                property real scaleY: 1 + (2.0 * animationFactor);
                property real scaleZ: 1 + (2.0 * animationFactor);

                property real rotationAngle: 360.0 *  animationFactor;
                property real rotationAxisX: 0.0;
                property real rotationAxisY: 0.0;
                property real rotationAxisZ: 1.0;

                property real translationX: 4 * (2.0 * (animationFactor  -0.5));
                property real translationY: 3 * (2.0 * (animationFactor - 0.5));
                property real translationZ: 0;

                transform: [
                    Scale3D {
                        scale: Qt.vector3d(testModel.scaleX,
                                           testModel.scaleY,
                                           testModel.scaleZ)
                    },
                    Rotation3D {
                        axis: Qt.vector3d(testModel.rotationAxisX,
                                          testModel.rotationAxisY,
                                          testModel.rotationAxisZ)
                        angle: testModel.rotationAngle
                    },
                    Translation3D {
                        translate: Qt.vector3d(testModel.translationX,
                                               testModel.translationY,
                                               testModel.translationZ)
                    }
                ]
            }
        }
    }
}
