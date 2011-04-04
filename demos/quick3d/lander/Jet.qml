import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Item3D {
    // We want to scale the flame, but keep the top constant so it
    // looks like it's attached to the lander's foot, so we use a
    // container Item3D to translate the flame's origin
    y: -0.97 - 0.25 + 0.5
    id: jet

    property real scaleFactor: 1.0
    property real internalScaleFactor: 1.0

    Behavior on scaleFactor { NumberAnimation { duration: 200 }}

    transform: Scale3D { scale: jet.scaleFactor * internalScaleFactor * 0.5 }

    SequentialAnimation {
        running:  true
        loops: Animation.Infinite
        NumberAnimation { target: jet;property: "internalScaleFactor";
            from: 1.0; to: 0.8; duration: 100 }
        PauseAnimation { duration: 100 }
        NumberAnimation { target: jet; property: "internalScaleFactor";
            from: 0.8; to: 1.0; duration: 100 }
        PauseAnimation { duration: 100 }
    }

    Quad {
        y: -1.0
        // set our top to be the lander's bottom:

        // Ah, billboards broken.
        //        transform: BillboardTransform {
        //            preserveUpVector: true
        //        }
        pretransform: [
            Rotation3D { axis: Qt.vector3d(1, 0, 0); angle: 90 }
        ]

        effect: flame
    }
}
