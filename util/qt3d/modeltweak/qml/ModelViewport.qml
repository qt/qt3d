import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0
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
    property string stateName;

    property variant upVector: Qt.vector3d(0, 1, 0)
    property variant rightVector: Qt.vector3d(0, 1, 0)

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

    property real cameraZoom: 1.0

    Viewport {
        id: viewport
        anchors.fill: parent
        picking: false
        blending: true

        // TODO: camera's position/rotation-around-origin/farplane should be alterable via gui
        camera: Camera {
            farPlane: 2000 // debugging
            projectionType: Camera.Orthographic
            viewSize: Qt.size(2*cameraZoom, 2*cameraZoom)
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

        Effect {
            id: coordLinesThickEffect
            color: "white"
        }
        Effect {
            id: coordLinesThinEffect
            color: "black"
        }

        // horizontal lines
        Line {
            id: lineAxisRight
            width: 3.0
            vertices: [
               cameraZoom*(-10*rightVector.x),                   cameraZoom*(-10*rightVector.y),                   cameraZoom*(-10*rightVector.z),
               cameraZoom*(10*rightVector.x),                    cameraZoom*(10*rightVector.y),                    cameraZoom*(10*rightVector.z),
               cameraZoom*(10*rightVector.x + 0.001*upVector.x), cameraZoom*(10*rightVector.y + 0.001*upVector.y), cameraZoom*(10*rightVector.z + 0.001*upVector.z)
            ]
            effect: coordLinesThickEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*rightVector.x + 0.25*upVector.x), cameraZoom*(-10*rightVector.y + 0.25*upVector.y), cameraZoom*(-10*rightVector.z + 0.25*upVector.z),
               cameraZoom*(10*rightVector.x + 0.25*upVector.x),  cameraZoom*(10*rightVector.y + 0.25*upVector.y),  cameraZoom*(10*rightVector.z + 0.25*upVector.z),
               cameraZoom*(10*rightVector.x + 0.251*upVector.x), cameraZoom*(10*rightVector.y + 0.251*upVector.y), cameraZoom*(10*rightVector.z + 0.251*upVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*rightVector.x + 0.5*upVector.x),  cameraZoom*(-10*rightVector.y + 0.5*upVector.y),  cameraZoom*(-10*rightVector.z + 0.5*upVector.z),
               cameraZoom*(10*rightVector.x + 0.5*upVector.x),   cameraZoom*(10*rightVector.y + 0.5*upVector.y),   cameraZoom*(10*rightVector.z + 0.5*upVector.z),
               cameraZoom*(10*rightVector.x + 0.501*upVector.x), cameraZoom*(10*rightVector.y + 0.501*upVector.y), cameraZoom*(10*rightVector.z + 0.501*upVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*rightVector.x + 0.75*upVector.x), cameraZoom*(-10*rightVector.y + 0.75*upVector.y), cameraZoom*(-10*rightVector.z + 0.75*upVector.z),
               cameraZoom*(10*rightVector.x + 0.75*upVector.x),  cameraZoom*(10*rightVector.y + 0.75*upVector.y),  cameraZoom*(10*rightVector.z + 0.75*upVector.z),
               cameraZoom*(10*rightVector.x + 0.751*upVector.x), cameraZoom*(10*rightVector.y + 0.751*upVector.y), cameraZoom*(10*rightVector.z + 0.751*upVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*rightVector.x - 0.25*upVector.x), cameraZoom*(-10*rightVector.y - 0.25*upVector.y), cameraZoom*(-10*rightVector.z - 0.25*upVector.z),
               cameraZoom*(10*rightVector.x - 0.25*upVector.x),  cameraZoom*(10*rightVector.y - 0.25*upVector.y),  cameraZoom*(10*rightVector.z - 0.25*upVector.z),
               cameraZoom*(10*rightVector.x - 0.251*upVector.x), cameraZoom*(10*rightVector.y - 0.251*upVector.y), cameraZoom*(10*rightVector.z - 0.251*upVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*rightVector.x - 0.5*upVector.x),  cameraZoom*(-10*rightVector.y - 0.5*upVector.y),  cameraZoom*(-10*rightVector.z - 0.5*upVector.z),
               cameraZoom*(10*rightVector.x - 0.5*upVector.x),   cameraZoom*(10*rightVector.y - 0.5*upVector.y),   cameraZoom*(10*rightVector.z - 0.5*upVector.z),
               cameraZoom*(10*rightVector.x - 0.501*upVector.x), cameraZoom*(10*rightVector.y - 0.501*upVector.y), cameraZoom*(10*rightVector.z - 0.501*upVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*rightVector.x - 0.75*upVector.x), cameraZoom*(-10*rightVector.y - 0.75*upVector.y), cameraZoom*(-10*rightVector.z - 0.75*upVector.z),
               cameraZoom*(10*rightVector.x - 0.75*upVector.x),  cameraZoom*(10*rightVector.y - 0.75*upVector.y),  cameraZoom*(10*rightVector.z - 0.75*upVector.z),
               cameraZoom*(10*rightVector.x - 0.751*upVector.x), cameraZoom*(10*rightVector.y - 0.751*upVector.y), cameraZoom*(10*rightVector.z - 0.751*upVector.z)
            ]
            effect: coordLinesThinEffect
        }

        // vertical lines
        Line {
            id: lineAxisUp
            width: 3.0
            vertices: [
               cameraZoom*(-10*upVector.x),                     cameraZoom*(-10*upVector.y),                     cameraZoom*(-10*upVector.z),
               cameraZoom*(10*upVector.x),                      cameraZoom*(10*upVector.y),                      cameraZoom*(10*upVector.z),
               cameraZoom*(10*upVector.x + 0.01*rightVector.x), cameraZoom*(10*upVector.y + 0.01*rightVector.y), cameraZoom*(10*upVector.z + 0.01*rightVector.z)
            ]
            effect: coordLinesThickEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*upVector.x + 0.25*rightVector.x), cameraZoom*(-10*upVector.y + 0.25*rightVector.y),  cameraZoom*(-10*upVector.z + 0.25*rightVector.z),
               cameraZoom*(10*upVector.x + 0.25*rightVector.x),  cameraZoom*(10*upVector.y + 0.25*rightVector.y),  cameraZoom*(10*upVector.z + 0.25*rightVector.z),
               cameraZoom*(10*upVector.x + 0.251*rightVector.x), cameraZoom*(10*upVector.y + 0.251*rightVector.y), cameraZoom*(10*upVector.z + 0.251*rightVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*upVector.x + 0.5*rightVector.x),  cameraZoom*(-10*upVector.y + 0.5*rightVector.y),   cameraZoom*(-10*upVector.z + 0.5*rightVector.z),
               cameraZoom*(10*upVector.x + 0.5*rightVector.x),   cameraZoom*(10*upVector.y + 0.5*rightVector.y),   cameraZoom*(10*upVector.z + 0.5*rightVector.z),
               cameraZoom*(10*upVector.x + 0.501*rightVector.x), cameraZoom*(10*upVector.y + 0.501*rightVector.y), cameraZoom*(10*upVector.z + 0.501*rightVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*upVector.x + 0.75*rightVector.x), cameraZoom*(-10*upVector.y + 0.75*rightVector.y), cameraZoom*(-10*upVector.z + 0.75*rightVector.z),
               cameraZoom*(10*upVector.x + 0.75*rightVector.x),  cameraZoom*(10*upVector.y + 0.75*rightVector.y),  cameraZoom*(10*upVector.z + 0.75*rightVector.z),
               cameraZoom*(10*upVector.x + 0.751*rightVector.x), cameraZoom*(10*upVector.y + 0.751*rightVector.y), cameraZoom*(10*upVector.z + 0.751*rightVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*upVector.x + 1.00*rightVector.x), cameraZoom*(-10*upVector.y + 1.00*rightVector.y), cameraZoom*(-10*upVector.z + 1.00*rightVector.z),
               cameraZoom*(10*upVector.x + 1.00*rightVector.x),  cameraZoom*(10*upVector.y + 1.00*rightVector.y),  cameraZoom*(10*upVector.z + 1.00*rightVector.z),
               cameraZoom*(10*upVector.x + 1.001*rightVector.x), cameraZoom*(10*upVector.y + 1.001*rightVector.y), cameraZoom*(10*upVector.z + 1.001*rightVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*upVector.x - 0.25*rightVector.x), cameraZoom*(-10*upVector.y - 0.25*rightVector.y), cameraZoom*(-10*upVector.z - 0.25*rightVector.z),
               cameraZoom*(10*upVector.x - 0.25*rightVector.x),  cameraZoom*(10*upVector.y - 0.25*rightVector.y),  cameraZoom*(10*upVector.z - 0.25*rightVector.z),
               cameraZoom*(10*upVector.x - 0.251*rightVector.x), cameraZoom*(10*upVector.y - 0.251*rightVector.y), cameraZoom*(10*upVector.z - 0.251*rightVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*upVector.x - 0.5*rightVector.x),  cameraZoom*(-10*upVector.y - 0.5*rightVector.y),  cameraZoom*(-10*upVector.z - 0.5*rightVector.z),
               cameraZoom*(10*upVector.x - 0.5*rightVector.x),   cameraZoom*(10*upVector.y - 0.5*rightVector.y),   cameraZoom*(10*upVector.z - 0.5*rightVector.z),
               cameraZoom*(10*upVector.x - 0.501*rightVector.x), cameraZoom*(10*upVector.y - 0.501*rightVector.y), cameraZoom*(10*upVector.z - 0.501*rightVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*upVector.x - 0.75*rightVector.x), cameraZoom*(-10*upVector.y - 0.75*rightVector.y), cameraZoom*(-10*upVector.z - 0.75*rightVector.z),
               cameraZoom*(10*upVector.x - 0.75*rightVector.x),  cameraZoom*(10*upVector.y - 0.75*rightVector.y),  cameraZoom*(10*upVector.z - 0.75*rightVector.z),
               cameraZoom*(10*upVector.x - 0.751*rightVector.x), cameraZoom*(10*upVector.y - 0.751*rightVector.y), cameraZoom*(10*upVector.z - 0.751*rightVector.z)
            ]
            effect: coordLinesThinEffect
        }
        Line {
            width: 1.0
            vertices: [
               cameraZoom*(-10*upVector.x - 1.00*rightVector.x), cameraZoom*(-10*upVector.y - 1.00*rightVector.y), cameraZoom*(-10*upVector.z - 1.00*rightVector.z),
               cameraZoom*(10*upVector.x - 1.00*rightVector.x),  cameraZoom*(10*upVector.y - 1.00*rightVector.y),  cameraZoom*(10*upVector.z - 1.00*rightVector.z),
               cameraZoom*(10*upVector.x - 1.001*rightVector.x), cameraZoom*(10*upVector.y - 1.001*rightVector.y), cameraZoom*(10*upVector.z - 1.001*rightVector.z)
            ]
            effect: coordLinesThinEffect
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

    ZoomControls {
        anchors {
            top: view.top; topMargin: 24;
            right: view.right; rightMargin: 2;
        }
        onZoomIn: {
            cameraZoom = 0.5 * cameraZoom;
        }
        onZoomOut: {
            cameraZoom = 2.0 * cameraZoom;
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

    // --------- grid text begin ---------

    property bool bTall: view.width<view.height;
    property int minimalDimension: (bTall) ? view.width : view.height;
    property int textStep: minimalDimension/8;

    // horizontal

    Text {
        id: h_minus100
        text: -1.0*cameraZoom
        x: view.width/2 - 4*textStep - 10
        y: (view.height/2) - 14
        color: "white"
        font.pixelSize: 12
        visible: bTall==false
    }
    Text {
        id: h_minus075
        text: -0.75*cameraZoom
        x: view.width/2 - 3*textStep - 10
        y: (view.height/2) - 14
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: h_minus050
        text: -0.5*cameraZoom
        x: view.width/2 - 2*textStep - 10
        y: (view.height/2) - 14
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: h_minus025
        text: -0.25*cameraZoom
        x: view.width/2 - textStep - 10
        y: (view.height/2) - 14
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: h_zero
        text: "0.0"
        x: (view.width/2) - 10
        y: (view.height/2) - 14
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: h_plus025
        text: 0.25*cameraZoom
        x: view.width/2 + textStep - 5
        y: (view.height/2) - 14
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: h_plus050
        text: 0.5*cameraZoom
        x: view.width/2 + 2*textStep - 5
        y: (view.height/2) - 14
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: h_plus075
        text: 0.75*cameraZoom
        x: view.width/2 + 3*textStep - 5
        y: (view.height/2) - 14
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: h_plus100
        text: 1.0*cameraZoom
        x: view.width/2 + 4*textStep
        y: (view.height/2) - 14
        color: "white"
        font.pixelSize: 12
        visible: bTall==false
    }

    // vertical

    Text {
        id: v_minus075
        text: -0.75*cameraZoom
        x: (view.width/2)
        y: view.height/2 - 3*textStep - 5
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: v_minus050
        text: -0.5*cameraZoom
        x: (view.width/2)
        y: view.height/2 - 2*textStep - 6
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: v_minus025
        text: -0.25*cameraZoom
        x: (view.width/2)
        y: view.height/2 - textStep - 7
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: v_plus025
        text: 0.25*cameraZoom
        x: (view.width/2)
        y: view.height/2 + textStep - 8
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: v_plus050
        text: 0.5*cameraZoom
        x: (view.width/2)
        y: view.height/2 + 2*textStep - 9
        color: "white"
        font.pixelSize: 12
    }
    Text {
        id: v_plus075
        text: 0.75*cameraZoom
        x: (view.width/2)
        y: view.height/2 + 3*textStep - 10
        color: "white"
        font.pixelSize: 12
    }

    // --------- grid text end ---------
}
