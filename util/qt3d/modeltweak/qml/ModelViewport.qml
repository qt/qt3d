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

    property double translateSensitivity: 32;
    property double rotateSensitivity: 8;
    property double scaleSensitivity: 32;
    property int downx;
    property int downy;
    property double originx;
    property double originy;
    property double originz;
    property double rotx;
    property double roty;
    property double rotz;
    property double scalex;
    property double scaley;
    property double scalez;

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
                transformRotateX,
                transformRotateY,
                transformRotateZ,
                transformScale
            ]
        }
    }
    MouseArea {
        property int mouseDown: Qt.NoButton

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

        onMouseXChanged: {
            if(mouseDown & Qt.LeftButton)
                moveMouseX(mouse);
            else if(mouseDown === Qt.RightButton)
                moveRotateX(mouse);
            else if(mouseDown === Qt.MiddleButton)
                scaleMouseX(mouse);
        }

        onMouseYChanged: {
            if(mouseDown & Qt.LeftButton)
                moveMouseY(mouse);
            else if(mouseDown === Qt.RightButton)
                moveRotateY(mouse);
            else if(mouseDown === Qt.MiddleButton)
                scaleMouseY(mouse);
        }

        onPressed: {
            // if we already have a mouse button down we don't want to do anything else until it's up again
            if(mouseDown !== Qt.NoButton)
                return;

            downx = mouse.x;
            downy = mouse.y;
            originx = transformTranslate.translate.x;
            originy = transformTranslate.translate.y;
            originz = transformTranslate.translate.z;
            rotx = transformRotateX.angle
            roty = transformRotateY.angle
            rotz = transformRotateZ.angle
            scalex = transformScale.scale.x
            scaley = transformScale.scale.y
            scalez = transformScale.scale.z

            mouseDown = mouse.button;
        }

        onReleased: { downx = 0; downy = 0; originx=0; originy=0; originz=0; mouseDown = Qt.NoButton }
    }
}
