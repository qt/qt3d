import QtQuick 1.0
import Qt3D 1.0

Rectangle {
    width: parent.width/2;
    height: parent.height/2
    color: parent.color
    border.color: parent.border.color

    property alias itemPosition: mainItem.position;
    property alias itemScale: mainItem.scale;
    property alias camera: viewport.camera

    Viewport {
        id: viewport
        anchors.fill: parent
        picking: false
        blending: true

        camera: Camera {
            farPlane: 2000 // debugging
        }
        navigation: false

        Item3D {
            id: mainItem
            mesh: source_mesh
            effect: Effect {}
            transform: [
                transformTranslate,
                transformRotate,
                transformScale
            ]
        }
    }
}
