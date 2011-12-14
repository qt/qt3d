import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0

Column {
    width: posX.width
    height: parent.height
    spacing: 4

    // POSITION
    Item {
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
            source: "images/lock.png"
        }
    }
    BlenderValueSlider {
        focus: true
        id: posX
        label: "X:"
        value: transformTranslate.translate.x.toFixed(3)
        function update (f)  {
            transformTranslate.translate = Qt.vector3d(f, transformTranslate.translate.y, transformTranslate.translate.z);
        }
        onNext: { updateMe(); focus = false; posY.focus = true; }
        onPrev: { updateMe(); focus = false; scaleZ.focus = true; }
    }
    BlenderValueSlider {
        id: posY
        label: "Y:"
        value: transformTranslate.translate.y.toFixed(3)
        function update (f)  {
            transformTranslate.translate = Qt.vector3d(transformTranslate.translate.x, f, transformTranslate.translate.z);
        }
        onNext: { updateMe(); focus = false; posZ.focus = true; }
        onPrev: { updateMe(); focus = false; posX.focus = true; }
    }
    BlenderValueSlider {
        id: posZ
        label: "Z:"
        value: transformTranslate.translate.z.toFixed(3)
        function update (f)  {
            transformTranslate.translate = Qt.vector3d(transformTranslate.translate.x, transformTranslate.translate.y, f);
        }
        onNext: { updateMe(); focus = false; rotX.focus = true; }
        onPrev: { updateMe(); focus = false; posY.focus = true; }
    }

    // ROTATE
    Item {
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
            source: "images/lock.png"
        }
    }
    BlenderValueSlider {
        id: rotX
        label: "X:"
        delta: 1
        min: 0;   limitMin: true
        max: 360; limitMax: true
        value: transformRotateX.angle.toFixed(3)
        function update (f)  { transformRotateX.angle = f }
        onNext: { updateMe(); focus = false; rotY.focus = true; }
        onPrev: { updateMe(); focus = false; posZ.focus = true; }
    }
    BlenderValueSlider {
        id: rotY
        label: "Y:"
        delta: 1
        min: 0;   limitMin: true
        max: 360; limitMax: true
        value: transformRotateY.angle.toFixed(3)
        function update (f)  { transformRotateY.angle = f }
        onNext: { updateMe(); focus = false; rotZ.focus = true; }
        onPrev: { updateMe(); focus = false; rotX.focus = true; }
    }
    BlenderValueSlider {
        id: rotZ
        label: "Z:"
        delta: 1
        min: 0;   limitMin: true
        max: 360; limitMax: true
        value: transformRotateZ.angle.toFixed(3)
        function update (f)  { transformRotateZ.angle = f }
        onNext: { updateMe(); focus = false; scaleX.focus = true; }
        onPrev: { updateMe(); focus = false; rotY.focus = true; }
    }

    // SCALE
    Item {
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
            source: "images/lock.png"
        }
    }
    BlenderValueSlider {
        id: scaleX
        label: "X:"
        min: 0; limitMin: true
        value: transformScale.scale.x.toFixed(3)
        function update (f)  { transformScale.scale = Qt.vector3d(f, transformScale.scale.y, transformScale.scale.z); }
        onNext: { updateMe(); focus = false; scaleY.focus = true; }
        onPrev: { updateMe(); focus = false; rotZ.focus = true; }
    }
    BlenderValueSlider {
        id: scaleY
        label: "Y:"
        min: 0; limitMin: true
        value: transformScale.scale.y.toFixed(3)
        function update (f)  { transformScale.scale = Qt.vector3d(transformScale.scale.x, f, transformScale.scale.z); }
        onNext: { updateMe(); focus = false; scaleZ.focus = true; }
        onPrev: { updateMe(); focus = false; scaleX.focus = true; }
    }
    BlenderValueSlider {
        id: scaleZ
        label: "Z:"
        min: 0; limitMin: true
        value: transformScale.scale.z.toFixed(3)
        function update (f)  { transformScale.scale = Qt.vector3d(transformScale.scale.x, transformScale.scale.y, f); }
        onNext: { updateMe(); focus = false; posX.focus = true; }
        onPrev: { updateMe(); focus = false; scaleY.focus = true; }
    }
}
