import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Viewport {
    width: 300
    height: 200
    Sphere {
        effect: Effect {
            texture: "moon-texture.jpg"
        }

        NumberAnimation {
            running: true
            target: rot;
            loops: Animation.Infinite
            property: "angle";
            to: 360; duration: 2000

        }


        transform: Rotation3D {
            id: rot
            axis: Qt.vector3d(0,1,0)
            angle: 0
        }
    }

    Quad {
        scale: 0.5

    }
}
