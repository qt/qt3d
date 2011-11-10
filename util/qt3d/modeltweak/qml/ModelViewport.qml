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

    property double sensitivity: 32;
    property int downx;
    property int downy;
    property double originx;
    property double originy;
    property double originz;

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
    MouseArea {
        anchors.fill: parent
        onMouseXChanged: moveMouseX(mouse)
        onMouseYChanged: moveMouseY(mouse)
        onPressed: { downx = mouse.x; downy = mouse.y; originx = transformTranslate.translate.x; originy = transformTranslate.translate.y; originz = transformTranslate.translate.z; }
        onReleased: { downx = 0; downy = 0; originx=0; originy=0; originz=0 }
    }
}
