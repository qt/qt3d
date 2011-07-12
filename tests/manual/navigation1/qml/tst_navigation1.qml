import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

// QTBUG-17408 TEST
Item {
    id: screen
    width: 720
    height: 480

    //ITEM 1: Simple model without materials
    Rectangle {
        id: panel1
        x: 0; y:0
        width:  screen.width/2
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            camera: Camera { eye: Qt.vector3d(0,3,15)}

            Item3D {
                scale: 2
                position: Qt.vector3d(0,-1,0)
                mesh: Mesh {source: "meshes/monkey.3ds"}
            }
        }

        Rectangle {
            border.width: 3; radius: 5
            width: parent.width
            height: text1.height + 10
            Text {
                id: text1
                width: parent.width
                wrapMode: "WordWrap"
                text: "  POSITIONAL ZOOM:\n\n" +
                "  A shaded lit monkey head should\n"+
                "  be displayed.  Left click-and-hold" +
                "  & move mouse to rotate.\n\n" +
                "  Wheel to zoom.  Zooming in should\n"+
                "  result in eventual clipping.";
                anchors.centerIn: parent
            }
        }
    }

    //ITEM 2: Simple model with materials
    Rectangle {
        id: panel2
        anchors.left: panel1.right
        anchors.top:  parent.top
        anchors.right:  parent.right
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            fovzoom: true
            camera: Camera { eye: Qt.vector3d(0,3,15)}

            Item3D {
                scale: 2
                position: Qt.vector3d(0,-1,0)
                mesh: Mesh {source: "meshes/monkey.3ds"}
            }
        }

        Rectangle {
            border.width: 3; radius: 5
            width: parent.width
            height: text2.height + 10
            Text {
                id: text2
                width: parent.width
                wrapMode: "WordWrap"
                 text: "  FIELD-OF-VIEW ZOOM:\n\n" +
                 "  A shaded lit monkey head should\n"+
                 "  be displayed.  Left click-and-hold" +
                 "  & move mouse to rotate.\n\n" +
                 "  Wheel to zoom.  Zooming in should\n"+
                 "  not result in eventual clipping.";
                anchors.centerIn: parent
            }
        }
    }    
}


