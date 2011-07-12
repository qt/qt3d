import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

// QTBUG-17408
Item {
    id: screen
    width: aTestCase.defaultWidth * 3
    height: aTestCase.defaultHeight * 3

    Flow {
        anchors.fill: parent

        TestCase {
            id: aTestCase
            text: "Rotation: X axis\n(Rolling towards you)"
            rotationAxis: Qt.vector3d(1,0,0)
        }

        TestCase {
            text: "Rotation on Y Axis\n(Cube front rotating left to right)"
            rotationAxis: Qt.vector3d(0,1,0)
        }

        TestCase {
            text: "Rotation: Z axis\n(Rolling to the left)"
            rotationAxis: Qt.vector3d(0,0,1)
        }


        TestCase {
            text: "Rotation: -X axis\n(Rolling away)"
            rotationAxis: Qt.vector3d(-1,0,0)
        }

        TestCase {
            text: "Rotation on -Y Axis\n(Cube front rotating right to left)"
            rotationAxis: Qt.vector3d(0,-1,0)
        }

        TestCase {
            text: "Rotation: -Z axis\n(Rolling to the right)"
            rotationAxis: Qt.vector3d(0,0,-1)
        }

        TestCase {
            text: "Rotation: 3 axis\n(top-right-front corner stationary)"
            rotationAxis: Qt.vector3d(1,1,1)
        }

        TestCase {
            text: "Axis Normalization\n(Identical to previous case)"
            rotationAxis: Qt.vector3d(12.3,12.3,12.3)
        }

        TestCase {
            text: "Degenerate Axis\n(Anything but a crash is ok)"
            rotationAxis: Qt.vector3d(0,0,0)
        }
    }
}
