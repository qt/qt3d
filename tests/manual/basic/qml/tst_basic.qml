import QtQuick 2.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Item {
    id: screen
    width: 640
    height: 520

    Viewport  {
        width: 640
        height: 480
        fillColor: "black"

        camera: Camera {eye: Qt.vector3d(0,3,20) }
        picking: true
        //showPicking: true

        ListModel {
            id: positionModel
            ListElement {
                x: -2.0; y: 1.8;
                color: "#CC2200";
                name_string: "top left"
            }
            ListElement {
                x: 0.0; y: 1.8;
                color: "#8822aa";
                name_string: "top middle"
            }
            ListElement {
                x: 2.0; y: 1.8;
                color: "#1122ca";
                name_string: "top right"
            }
            ListElement {
                x: -2.0; y: 0.0;
                color: "#CC8800";
                name_string: "mid left"
            }
            ListElement {
                x: 0.0; y: 0.0;
                color: "#8888aa";
                name_string: "middle"
            }
            ListElement {
                x: 2.0; y: 0.0;
                color: "#118800";
                name_string: "mid right"
            }
            ListElement {
                x: -2.0; y: -1.8;
                color: "#CCBB00";
                name_string: "bottom left"
            }
            ListElement {
                x: 0.0; y: -1.8;
                color: "#88BB00";
                name_string: "bottom middle"
            }
            ListElement {
                x: 2.0; y: -1.8;
                color: "#11BB00";
                name_string: "bottom right"
            }
        }

        Component {
            id: cubeComponent
            Cube {
                position: Qt.vector3d(model.x,model.y, 0.0)
                onClicked: console.log(model.name_string)
                effect: Effect {
                    color: model.color
                }
            }
        }

        Item3D {
            Repeater {
                delegate: cubeComponent
                model: positionModel
            }
        }
    }
    Rectangle {
        id: instructions
        x: 0; y: 480
        width: screen.width
        height: 40
        color: "#3333aa"
        Text {
            id: instructionsText
            wrapMode: "WordWrap"
            text: "Pick a cube, check log output. Drag in the black area to move all cubes."
            anchors.centerIn: parent
        }
    }
}

