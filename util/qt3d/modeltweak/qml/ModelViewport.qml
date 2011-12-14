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
    property alias viewportName: viewportText.text;

    // the current x/y positions of the mouse when the onPressed event was triggered;
    // values are invalid if onRelease has occured
    property variant down;

    // current xyz values of the Translation3D/Rotation3Ds/Scale3D when onPressed was triggered;
    // values are invalid if onRelease has occured
    property variant translate;
    property variant rotate;
    property variant scale3d;

    // the pixel sensitivity values of the mousemovements
    // TODO: expose via api
    // TODO: maybe save in settings api?
    property double translateSensitivity: 32;
    property double rotateSensitivity: 8;
    property double scaleSensitivity: 32;

    // the name of this viewport's state
    property string stateName

    // the smoothness values for the state change transformations
    // TODO: this should be alterable via the gui and saved in a settings file
    Behavior on x      { NumberAnimation { duration: 300 } }
    Behavior on y      { NumberAnimation { duration: 300 } }
    Behavior on width  { NumberAnimation { duration: 300 } }
    Behavior on height { NumberAnimation { duration: 300 } }

    signal mouseTranslateX(variant mouse)
    signal mouseTranslateY(variant mouse)
    signal mouseRotateX(variant mouse)
    signal mouseRotateY(variant mouse)
    signal mouseScaleX(variant mouse)
    signal mouseScaleY(variant mouse)

    Viewport {
        id: viewport
        anchors.fill: parent
        picking: false
        blending: true

        // TODO: camera's position/rotation-around-origin/farplane should be alterable via gui
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
                mouseTranslateX(mouse)
            else if (mouseDown === Qt.RightButton)
                mouseRotateX(mouse);
            else if (mouseDown === Qt.MiddleButton)
                mouseScaleX(mouse)
        }

        onMouseYChanged: {
            if (mouseDown & Qt.LeftButton)
                mouseTranslateY(mouse)
            else if (mouseDown === Qt.RightButton)
                mouseRotateY(mouse);
            else if (mouseDown === Qt.MiddleButton)
                mouseScaleY(mouse)
        }

        onPressed: {
            // if we already have a mouse button down we don't want to do anything else until it's up again
            if (mouseDown !== Qt.NoButton)
                return;

            mouseDown = mouse.button;
            down      = Qt.point(mouse.x, mouse.y)
            translate = transformTranslate.translate
            rotate    = Qt.vector3d(transformRotateX.angle, transformRotateY.angle, transformRotateZ.angle)
            scale3d   = transformScale.scale
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

    Text {
        id: viewportText
        anchors {
            top: view.top; topMargin: 4;
            left: view.left; leftMargin: 4;
        }
        color: "white"
        font.pixelSize: 16
    }
}
