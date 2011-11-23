import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0
import "Widgets"

Rectangle {
    id: view
    width: parent.width/2;
    height: parent.height/2

    color: parent.color
    border.color: parent.border.color

    property alias itemPosition: mainItem.position;
    property alias itemScale: mainItem.scale;
    property alias camera: viewport.camera

    // the current x/y positions of the mouse when the onPressed event was triggered;
    // values are invalid if onRelease has occured
    property int downX;
    property int downY;

    // current xyz positions of the Translation3D when onPressed was triggered;
    // values are invalid if onRelease has occured
    property double translateX;
    property double translateY;
    property double translateZ;

    // current angles of the three Rotation3Ds when onPressed was triggered;
    // values are invalid if onRelease has occured
    property double rotateX;
    property double rotateY;
    property double rotateZ;

    // current xyz sizes of the Scale3D when onPressed was triggered;
    // values are invalid if onRelease has occured
    property double scaleX;
    property double scaleY;
    property double scaleZ;

    // the pixel sensitivity values of the mousemovements
    // TODO: expose via api
    // TODO: maybe save in settings api?
    property double translateSensitivity: 32;
    property double rotateSensitivity: 8;
    property double scaleSensitivity: 32;

    property string stateName

    Behavior on x      { NumberAnimation { duration: 300 } }
    Behavior on y      { NumberAnimation { duration: 300 } }
    Behavior on width  { NumberAnimation { duration: 300 } }
    Behavior on height { NumberAnimation { duration: 300 } }

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
            transform: [
                transformScale,
                transformRotateX,
                transformRotateY,
                transformRotateZ,
                transformTranslate,
            ]
        }
    }

    MouseArea {
        property int mouseDown: Qt.NoButton

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

        onMouseXChanged: {
            if (mouseDown & Qt.LeftButton)
                translateMouseX(mouse);
            else if (mouseDown === Qt.RightButton)
                rotateMouseX(mouse);
            else if (mouseDown === Qt.MiddleButton)
                scaleMouseX(mouse);
        }

        onMouseYChanged: {
            if (mouseDown & Qt.LeftButton)
                translateMouseY(mouse);
            else if (mouseDown === Qt.RightButton)
                rotateMouseY(mouse);
            else if (mouseDown === Qt.MiddleButton)
                scaleMouseY(mouse);
        }

        onPressed: {
            // if we already have a mouse button down we don't want to do anything else until it's up again
            if (mouseDown !== Qt.NoButton)
                return;

            mouseDown = mouse.button;

            downX = mouse.x;
            downY = mouse.y;

            if (mouseDown & Qt.LeftButton) {
                translateX = transformTranslate.translate.x;
                translateY = transformTranslate.translate.y;
                translateZ = transformTranslate.translate.z;
            }

            if (mouseDown & Qt.RightButton) {
                rotateX = transformRotateX.angle
                rotateY = transformRotateY.angle
                rotateZ = transformRotateZ.angle
            }
            if (mouseDown & Qt.MiddleButton) {
                scaleX = transformScale.scale.x
                scaleY = transformScale.scale.y
                scaleZ = transformScale.scale.z
            }
        }

        // clear the current mouse button upon release
        onReleased: { mouseDown = Qt.NoButton }
    }

    ModelViewportResize {
        anchors {
            top: view.top; topMargin: 2;
            right: view.right; rightMargin: 2;
        }
    }
}
