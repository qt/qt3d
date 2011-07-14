import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

// QTBUG-17408
Item {
    id: screen
    width: aTestCase.defaultWidth * 3
    height: aTestCase.defaultHeight * 4
    property variant scalingCamera: Camera {eye: Qt.vector3d(0,20,20) }

    Flow {
        anchors.fill: parent

        ScalingTestCase {
            id: aTestCase
            text: "Scaling up"
            camera: scalingCamera
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(4,4,4)
        }

        ScalingTestCase {
            text: "Scaling down"
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(0.1,0.1,0.1)
        }

        ScalingTestCase {
            text: "Scaling using real\n(scaling and inversion)"
            testTransform: Scale3D {}
            camera: scalingCamera
            scaleFactor: 1
            SequentialAnimation on scaleFactor {
                loops: Animation.Infinite
                PropertyAnimation {
                    from: -3
                    to: 3
                    duration: 1500
                }
                PropertyAnimation {
                    from: 3
                    to: -3
                    duration: 1500
                }
            }
        }

        ScalingTestCase {
            text: "Scaling in X\n(mirroring horizontally)"
            camera: scalingCamera
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(4,1,1)
        }

        ScalingTestCase {
            text: "Scaling in Y\n(stretching vertically)"
            camera: scalingCamera
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(1,4,1)
        }

        ScalingTestCase {
            text: "Scaling in Z\n(depth-wise)"
            camera: scalingCamera
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(1,1,4)
        }

        ScalingTestCase {
            text: "Negative Scaling in X\n(mirroring horizontally)"
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(-1,1,1)
        }

        ScalingTestCase {
            text: "Negative Scaling in Y\n(flipping upside-down)"
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(1,-1,1)
        }

        ScalingTestCase {
            text: "Negative Scaling in Z\n(depth-wise)"
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(1,1,-1)
        }

        ScalingTestCase {
            text: "Zero Z\n(Probable artifacts)"
            scaleVectorA: Qt.vector3d(1,1,0)
            scaleVectorB: Qt.vector3d(1,1,0)
        }

        ScalingTestCase {
            text: "Zero Y\n(Probable artifacts)"
            scaleVectorA: Qt.vector3d(1,0,1)
            scaleVectorB: Qt.vector3d(1,0,1)
        }

        // Skipping zero X because it would be invisible  with the
        // camera position all the other tests have

        ScalingTestCase {
            text: "Zero Vector\n(Not visible and shouldn't crash)"
            scaleVectorA: Qt.vector3d(0,0,0)
            scaleVectorB: Qt.vector3d(0,0,0)
        }
    }
}
