import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

// QTBUG-17392 / QTBUG-17391
Item {
    id: screen
    width: 720
    height: 480

    //ITEM 1: Simple model without materials
    Rectangle {
        id: panel1
        x: 0; y:0
        width:  screen.width/4
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
                text: "  MULTI-COMPONENT:  No\n  MATERIALS:  None\n\n  A grey shaded monkey head\n  should be visible.\n\n  Use mouse to rotate/zoom."
                anchors.centerIn: parent
            }
        }
    }

    //ITEM 2: Simple model with materials
    Rectangle {
        id: panel2
        anchors.left: panel1.right
        anchors.top:  parent.top
        width:  screen.width/4
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            camera: Camera { eye: Qt.vector3d(0,3,15)}

            Item3D {
                scale: 0.8
                position: Qt.vector3d(0,-1,0)
                mesh: Mesh {source: "meshes/penguin.3ds"}
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
                 text: "  MULTI-COMPONENT:  No\n  MATERIALS:  Yes\n\n  A coloured, lit penguin\n  should be visible.\n\n  Use mouse to rotate/zoom."
                anchors.centerIn: parent
            }
        }
    }

    //ITEM 3: A complex model without materials
    Rectangle {
        id: panel3
        anchors.left: panel2.right
        anchors.top:  parent.top
        width:  screen.width/4
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            camera: Camera { eye: Qt.vector3d(0,0,10)}

            Item3D {
                scale: 0.4
                position: Qt.vector3d(0,-1,0)
                mesh: Mesh {source: "meshes/lunar-lander.3ds"}
            }
        }
        //text for item3
        Rectangle {
            border.width: 3; radius: 5
            width: parent.width
            height: text3.height + 10
            Text {
                id: text3
                width: parent.width
                wrapMode: "WordWrap"
                text: "  MULTI-COMPONENT:  Yes\n  MATERIALS:  None\n\n  A grey shaded lunar lander\n  should be visible.\n\n  Use mouse to rotate/zoom."
                anchors.centerIn: parent
            }
        }
    }
    //ITEM 4: A Complex model with materials
    Rectangle {
        id: panel4
        anchors.left: panel3.right
        anchors.top:  parent.top
        width:  screen.width/4
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            camera: Camera { eye: Qt.vector3d(0,0,10)}

            Item3D {
                scale: 0.4
                position: Qt.vector3d(0,-2,0)

                transform: [
                    Rotation3D {id: correct; angle: 90; axis: Qt.vector3d(0,1,0)},
                    Rotation3D {id: pitch; angle: -90; axis: Qt.vector3d(0,0,1)},
                    Rotation3D {id: roll; angle: 0; axis: Qt.vector3d(1,0,0)},
                    Rotation3D {id: yaw; angle: 0; axis: Qt.vector3d(0,1,0)}
                ]

                mesh: Mesh {source: "meshes/robot.3ds"}
            }
        }
        //text for item4
        Rectangle {
            border.width: 3; radius: 5
            width: parent.width
            height: text4.height + 10
            Text {
                id: text4
                width: parent.width
                wrapMode: "WordWrap"
                text: "  MULTI-COMPONENT:  Yes\n  MATERIALS:  Yes\n\n  A lit, coloured, textured robot\n  should be visible.\n\n  Use mouse to rotate/zoom."
                anchors.centerIn: parent
            }
        }
    }
}


