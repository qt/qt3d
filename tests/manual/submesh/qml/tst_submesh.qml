import Qt 4.7
import Qt3D 1.0

// QTBUG-17418
Item {
    id: screen
    width: 720
    height: 480
    Rectangle {
            id: description
            border.width: 3
            radius: 5
            property variant radarPostion: recieverItem.localToWorld();
            x: radarPostion.x * view.width
            width: parent.width
            height: text.height + 10
            Text {
                id: text
                width: parent.width
                wrapMode: "WordWrap"
                text: "There should be a stationary lander model with a rotating radar dish attached.  \n" +
                      "The lander and attached radar dish should show no scaling or translation.\n" +
                      "There should be a second radar dish, scaling but not rotating.. \n" +
                      "It is acceptable if the scaling dish appears to be moving due to the scaling.\n" +
                      "There should be a third radar dish colored red, oscillating horizontally without scaling."
                anchors.centerIn: parent
            }
        }
        Rectangle {
            width: parent.width
            anchors.top: description.bottom
            anchors.bottom: screen.bottom
            anchors.left: screen.left
            anchors.right: screen.right
            color: "#aaccee"

            Viewport  {
                id: view
                anchors.fill: parent
                picking: true
                //        showPicking: true
                camera: Camera { eye: Qt.vector3d(0,15,50)}

                Mesh {
                    id:landerMesh
                    source: "meshes/lunar-lander.3ds"
                }

                Item3D {
                    mesh: landerMesh

                    Item3D {
                        // Correct context loss from "pulling off" of graph
                        transform: [
                            Rotation3D { axis: Qt.vector3d(1.0, 0.0, 0.0) ; angle: -90},
                            Rotation3D {
                                id: radarSpin2 ; axis: Qt.vector3d(0.0, 1.0, 0.0);
                                angle: 0
                                origin: Qt.vector3d(0.55,0.0,0.0)
                                NumberAnimation on angle {
                                    running: true; loops: Animation.Infinite
                                    from: 360; to: 0; duration: 3000;
                                }
                            }
                        ]

                        Item3D { mesh: landerMesh; meshNode: "Rod.001" }
                        Item3D { mesh: landerMesh; meshNode: "Receiver.001" }
                        Item3D { mesh: landerMesh; meshNode: "Dish.001" }
                    }
                }

                Item3D {
                    // Check that you can duplicate items
                    id: scalingRadarDish
                    SequentialAnimation on scale {
                        loops: Animation.Infinite; running: true
                        NumberAnimation { from:1.0 ; to: 3.0; duration: 500 }
                        NumberAnimation { from:3.0 ; to: 1.0; duration: 500 }

                    }
                    // rotate object to remove ambiguity - scaling causes
                    // movement, and if this is in the z axis, the scaling
                    // could be confused with perspective.

                    position: Qt.vector3d(3.0,0.0,0.0)

                    transform: Rotation3D {
                        axis: Qt.vector3d(0.0,1.0,0.0)
                        angle: -90
                    }

                    effect:
                        Effect { id: redEffect; material: red }
                    Material {
                        // Distinctive red Color
                        id: red
                        ambientColor: "#884444"
                        diffuseColor: "#EE4422"
                        specularColor: "#FF8888"
                        shininess: 200
                    }

                    Item3D { mesh: landerMesh; meshNode: "Rod.001"; effect: redEffect }
                    Item3D { id: recieverItem; mesh: landerMesh; meshNode: "Receiver.001" ; effect: redEffect }
                    Item3D { mesh: landerMesh; meshNode: "Dish.001" ; effect: redEffect }

                }
                Item3D {
                    // Check that you can duplicate items
                    id: translatingRadarDish

                    SequentialAnimation on x {
                        loops: Animation.Infinite
                        NumberAnimation { from: 0.0; to: 5.0 ; duration: 600 }
                        NumberAnimation { from: 5.0; to: 0.0 ; duration: 600 }
                    }

                    Item3D { mesh: landerMesh; meshNode: "Rod.001"; effect: redEffect }
                    Item3D { mesh: landerMesh; meshNode: "Receiver.001" ; effect: redEffect }
                    Item3D { mesh: landerMesh; meshNode: "Dish.001" ; effect: redEffect }

                }
            }

     }

}
