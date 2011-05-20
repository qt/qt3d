import Qt 4.7
import Qt3D 1.0
import Qt3D.Shapes 1.0

// QTBUG-18702
Item {
    id: screen
    width: aTestCase.defaultWidth * 3
    height: aTestCase.defaultHeight * 3

    Flow {
        anchors.fill: parent

        TestCase {
            id: aTestCase
            text: "Rotation: Z axis\n(Rolling to the left)\nNumberAnimation"
            rotationAxis: Qt.vector3d(0,0,1)
            rotation: Rotation3D {
                axis: rotationAxis
                NumberAnimation on angle {
                    loops: Animation.Infinite
                    from: 0
                    to: 360
                    duration: 1500
                }
            }
        }

        TestCase {
            text: "Rotation: Z axis\n(Rolling to the left)\nRotationAnimation"
            rotationAxis: Qt.vector3d(0,0,1)
            rotation: Rotation3D {
                axis: rotationAxis
                RotationAnimation on angle {
                    loops: Animation.Infinite
                    from: 0
                    to: 359
                    duration: 1500
                    direction: RotationAnimation.Clockwise
                }
            }
        }

        TestCase {
            text: "Rotation: Z axis\n(Rolling to the left)\nSmoothedAnimation"
            rotationAxis: Qt.vector3d(0,0,1)
            rotation: Rotation3D {
                axis: rotationAxis
                SmoothedAnimation on angle {
                    loops: Animation.Infinite
                    from: 0
                    to: 360
                    duration: 1500
                    velocity: 200
                }
            }
        }

        TestCase {
            text: "Rotation: Z axis\n(Rolling to the left)\nSpringAnimation"
            rotationAxis: Qt.vector3d(0,0,1)
            rotation: Rotation3D {
                axis: rotationAxis
                SpringAnimation on angle {
                    id: rotationAnimation
                    loops: Animation.Infinite
                    from: 0
                    to: 359
                    duration: 1500
                    spring: 2
                    damping: 0.2
                }
            }
        }
    }
}
