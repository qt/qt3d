import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0

Column {
    width: posX.width
    height: parent.height
    spacing: 4

    property alias rotateLocked: imageR.isLocked
    property alias scaleLocked: imageS.isLocked
    property alias translateLocked: imageP.isLocked;

    // POSITION
    Item {
        id: positionPanel
        width: parent.width
        height: imageP.height

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 8
            text: "Position";
            color: "#FFFFFF"
        }
        Image {
            id: imageP
            anchors.right: parent.right
            anchors.rightMargin: 8

            //Animation to pulse the lock icon if attempting to modify while locked.
            property bool bounce: false

            SequentialAnimation on scale{
                running: imageP.bounce
                NumberAnimation { to : 2.0; duration: 150; easing.type: "OutQuad" }
                NumberAnimation { to : 1.0; duration: 150; easing.type: "OutQuad" }
                NumberAnimation { to : 2.0; duration: 150; easing.type: "OutQuad" }
                NumberAnimation { to : 1.0; duration: 150; easing.type: "OutQuad" }
                onCompleted: imageP.bounce = false
            }

            //Manage locked/unlocked state
            state: "UNLOCKED"
            property bool isLocked: false

            states: [
                State {
                    name: "UNLOCKED"
                    PropertyChanges { target: imageP; source: "images/unlock.png"}
                    PropertyChanges { target: imageP; isLocked: false;}
                },
                State {
                    name: "LOCKED"
                    PropertyChanges { target: imageP; source: "images/lock.png"}
                    PropertyChanges { target: imageP; isLocked: true;}
                }
            ]

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: {
                    if (parent.state=="LOCKED")
                        parent.state="UNLOCKED"
                    else
                        parent.state="LOCKED"
                }
            }
        }
    }
    BlenderValueSlider {
        focus: true
        id: posX
        label: "X:"
        locked: imageP.isLocked
        value: transformTranslate.translate.x.toFixed(3)
        function update (f)  {
            transformTranslate.translate = Qt.vector3d(f, transformTranslate.translate.y, transformTranslate.translate.z);
        }
        onNext: { updateMe(); focus = false; posY.focus = true; }
        onPrev: { updateMe(); focus = false; scaleZ.focus = true; }
        onFail: { imageP.bounce=true; }
    }
    BlenderValueSlider {
        id: posY
        label: "Y:"
        locked: imageP.isLocked
        value: transformTranslate.translate.y.toFixed(3)
        function update (f)  {
            transformTranslate.translate = Qt.vector3d(transformTranslate.translate.x, f, transformTranslate.translate.z);
        }
        onNext: { updateMe(); focus = false; posZ.focus = true; }
        onPrev: { updateMe(); focus = false; posX.focus = true; }
        onFail: { imageP.bounce=true; }
    }
    BlenderValueSlider {
        id: posZ
        label: "Z:"
        locked: imageP.isLocked
        value: transformTranslate.translate.z.toFixed(3)
        function update (f)  {
            transformTranslate.translate = Qt.vector3d(transformTranslate.translate.x, transformTranslate.translate.y, f);
        }
        onNext: { updateMe(); focus = false; rotX.focus = true; }
        onPrev: { updateMe(); focus = false; posY.focus = true; }
        onFail: { imageP.bounce=true; }
    }

    // ROTATE
    Item {
        id: rotationPanel
        width: parent.width
        height: imageR.height

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 8
            text: "Rotation";
            color: "#FFFFFF"
        }
        Image {
            id: imageR
            anchors.right: parent.right
            anchors.rightMargin: 8

            //Animation to pulse the lock icon if attempting to modify while locked.
            property bool bounce: false

            SequentialAnimation on scale{
                running: imageR.bounce
                NumberAnimation { to : 2.0; duration: 150; easing.type: "OutQuad" }
                NumberAnimation { to : 1.0; duration: 150; easing.type: "OutQuad" }
                NumberAnimation { to : 2.0; duration: 150; easing.type: "OutQuad" }
                NumberAnimation { to : 1.0; duration: 150; easing.type: "OutQuad" }
                onCompleted: imageR.bounce = false
            }

            //Manage locked/unlocked state
            state: "UNLOCKED"
            property bool isLocked: false

            states: [
                State {
                    name: "UNLOCKED"
                    PropertyChanges { target: imageR; source: "images/unlock.png"}
                    PropertyChanges { target: imageR; isLocked: false;}
                },
                State {
                    name: "LOCKED"
                    PropertyChanges { target: imageR; source: "images/lock.png"}
                    PropertyChanges { target: imageR; isLocked: true;}
                }
            ]

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: {
                    if (parent.state=="LOCKED")
                        parent.state="UNLOCKED"
                    else
                        parent.state="LOCKED"
                }
            }
        }
    }
    BlenderValueSlider {
        id: rotX
        label: "X:"
        delta: 1
        locked: imageR.isLocked
        min: 0;   limitMin: true
        max: 360; limitMax: true
        value: transformRotateX.angle.toFixed(3)
        function update (f)  { transformRotateX.angle = f }
        onNext: { updateMe(); focus = false; rotY.focus = true; }
        onPrev: { updateMe(); focus = false; posZ.focus = true; }
        onFail: { imageR.bounce=true; }
    }
    BlenderValueSlider {
        id: rotY
        label: "Y:"
        delta: 1
        locked: imageR.isLocked
        min: 0;   limitMin: true
        max: 360; limitMax: true
        value: transformRotateY.angle.toFixed(3)
        function update (f)  { transformRotateY.angle = f }
        onNext: { updateMe(); focus = false; rotZ.focus = true; }
        onPrev: { updateMe(); focus = false; rotX.focus = true; }
        onFail: { imageR.bounce=true; }
    }
    BlenderValueSlider {
        id: rotZ
        label: "Z:"
        delta: 1
        locked: imageR.isLocked
        min: 0;   limitMin: true
        max: 360; limitMax: true
        value: transformRotateZ.angle.toFixed(3)
        function update (f)  { transformRotateZ.angle = f }
        onNext: { updateMe(); focus = false; scaleX.focus = true; }
        onPrev: { updateMe(); focus = false; rotY.focus = true; }
        onFail: { imageR.bounce=true; }
    }

    // SCALE
    Item {
        id: scalePanel
        width: parent.width
        height: imageS.height

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 8
            text: "Scale";
            color: "#FFFFFF"
        }
        Image {
            id: imageS
            anchors.right: parent.right
            anchors.rightMargin: 8

            //Animation to pulse the lock icon if attempting to modify while locked.
            property bool bounce: false

            SequentialAnimation on scale{
                running: imageS.bounce
                NumberAnimation { to : 2.0; duration: 150; easing.type: "OutQuad" }
                NumberAnimation { to : 1.0; duration: 150; easing.type: "OutQuad" }
                NumberAnimation { to : 2.0; duration: 150; easing.type: "OutQuad" }
                NumberAnimation { to : 1.0; duration: 150; easing.type: "OutQuad" }
                onCompleted: imageS.bounce = false
            }

            //Manage locked/unlocked state
            state: "UNLOCKED"
            property bool isLocked: false

            states: [
                State {
                    name: "UNLOCKED"
                    PropertyChanges { target: imageS; source: "images/unlock.png"}
                    PropertyChanges { target: imageS; isLocked: false;}
                },
                State {
                    name: "LOCKED"
                    PropertyChanges { target: imageS; source: "images/lock.png"}
                    PropertyChanges { target: imageS; isLocked: true;}
                }
            ]

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: {
                    if (parent.state=="LOCKED")
                        parent.state="UNLOCKED"
                    else
                        parent.state="LOCKED"
                }
            }
        }
    }
    BlenderValueSlider {
        id: scaleX
        label: "X:"
        locked: imageS.isLocked
        min: 0; limitMin: true
        value: transformScale.scale.x.toFixed(3)
        function update (f)  { transformScale.scale = Qt.vector3d(f, transformScale.scale.y, transformScale.scale.z); }
        onNext: { updateMe(); focus = false; scaleY.focus = true; }
        onPrev: { updateMe(); focus = false; rotZ.focus = true; }
        onFail: { imageS.bounce=true; }
    }
    BlenderValueSlider {
        id: scaleY
        label: "Y:"
        locked: imageS.isLocked
        min: 0; limitMin: true
        value: transformScale.scale.y.toFixed(3)
        function update (f)  { transformScale.scale = Qt.vector3d(transformScale.scale.x, f, transformScale.scale.z); }
        onNext: { updateMe(); focus = false; scaleZ.focus = true; }
        onPrev: { updateMe(); focus = false; scaleX.focus = true; }
        onFail: { imageS.bounce=true; }
    }
    BlenderValueSlider {
        id: scaleZ
        label: "Z:"
        locked: imageS.isLocked
        min: 0; limitMin: true
        value: transformScale.scale.z.toFixed(3)
        function update (f)  { transformScale.scale = Qt.vector3d(transformScale.scale.x, transformScale.scale.y, f); }
        onNext: { updateMe(); focus = false; posX.focus = true; }
        onPrev: { updateMe(); focus = false; scaleY.focus = true; }
        onFail: { imageS.bounce=true; }
    }
}
