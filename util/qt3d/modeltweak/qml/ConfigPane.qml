import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0

Column {
    anchors.left: parent.left
    anchors.leftMargin: 32
    anchors.topMargin: 32
    y: parent.height/2 + 32
    width: 200
    height: 200

    // POSITION
    Text {
        text: "Position";
        color: "#FFFFFF"
        anchors.horizontalCenter: parent.horizontalCenter
    }
    ValueField {
        focus: true
        id: posX
        label: "X:"
        value: transformTranslate.translate.x
        function update (f)  {
            transformTranslate.translate = Qt.vector3d(f, transformTranslate.translate.y, transformTranslate.translate.z);
        }
        Keys.onTabPressed:   { updateMe(); posY.focus = true; }
    }
    ValueField {
        id: posY
        label: "Y:"
        value: transformTranslate.translate.y
        function update (f)  {
            transformTranslate.translate = Qt.vector3d(transformTranslate.translate.x, f, transformTranslate.translate.z);
        }
        Keys.onTabPressed:   { updateMe(); posZ.focus = true; }
    }
    ValueField {
        id: posZ
        label: "Z:"
        value: transformTranslate.translate.z
        function update (f)  {
            transformTranslate.translate = Qt.vector3d(transformTranslate.translate.x, transformTranslate.translate.y, f);
        }
        Keys.onTabPressed:   { updateMe(); rotX.focus = true; }
    }

    // ROTATE
    Text {
        text: "Rotation";
        color: "#FFFFFF"
        anchors.horizontalCenter: parent.horizontalCenter
    }
    ValueField {
        id: rotX
        label: "X:"
        value: transformRotate.axis.x
        function update (f)  { transformRotate.axis.x = f }
        Keys.onTabPressed:   { updateMe(); rotY.focus = true; }
    }
    ValueField {
        id: rotY
        label: "Y:"
        value: transformRotate.axis.y
        function update (f)  { transformRotate.axis.y = f; }
        Keys.onTabPressed:   { updateMe(); rotZ.focus = true; }
    }
    ValueField {
        id: rotZ
        label: "Z:"
        value: transformRotate.axis.z
        function update (f)  { transformRotate.axis.z = f }
        Keys.onTabPressed:   { updateMe(); angle.focus = true; }
    }
    ValueField {
        id: angle
        label: "Angle:"
        delta: 1
        min: 0;   limitMin: true
        max: 360; limitMax: true
        value: transformRotate.angle
        function update (f)  { transformRotate.angle = f }
        Keys.onTabPressed:   { updateMe(); scaleX.focus = true; }
    }

    // SCALE
    Text {
        text: "Scale";
        color: "#FFFFFF"
        anchors.horizontalCenter: parent.horizontalCenter
    }
    ValueField {
        id: scaleX
        label: "X:"
        min: 0; limitMin: true
        value: transformScale.scale.x
        function update (f)  { transformScale.scale = Qt.vector3d(f, transformScale.scale.y, transformScale.scale.z); }
        Keys.onTabPressed:   { updateMe(); scaleY.focus = true; }
    }
    ValueField {
        id: scaleY
        label: "Y:"
        min: 0; limitMin: true
        value: transformScale.scale.y
        function update (f)  { transformScale.scale = Qt.vector3d(transformScale.scale.x, f, transformScale.scale.z); }
        Keys.onTabPressed:   { updateMe(); scaleZ.focus = true; }
    }
    ValueField {
        id: scaleZ
        label: "Z:"
        min: 0; limitMin: true
        value: transformScale.scale.z
        function update (f)  { transformScale.scale = Qt.vector3d(transformScale.scale.x, transformScale.scale.y, f); }
        Keys.onTabPressed:   { updateMe(); posX.focus = true; }
    }

    // CAMERA
    Text {
        text: "Camera";
        color: "#FFFFFF"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle {
        anchors.topMargin: 8
        width: parent.width
        height: 30
        Text {
            anchors.fill: parent
            text: "Click to Save!"
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                var saveData =
                        "import QtQuick 1.0\n" +
                        "import Qt3D 1.0\n" +
                        "\n" +
                        "Item3D {\n" +
                        "    Translation3D {\n" +
                        "        id: transformTranslate\n" +
                        "        translate: Qt.vector3d(" +
                            transformTranslate.translate.x + ", " +
                            transformTranslate.translate.y + ", " +
                            transformTranslate.translate.z + ")\n" +
                        "    }\n" +
                        "\n" +
                        "    Rotation3D {\n" +
                        "        id: transformRotate\n" +
                        "        angle: " + transformRotate.angle + "\n" +
                        "        axis: Qt.vector3d(" +
                            transformRotate.axis.x + ", " +
                            transformRotate.axis.y + ", " +
                            transformRotate.axis.z + ")\n" +
                        "    }\n" +
                        "\n" +
                        "    Scale3D {\n" +
                        "        id: transformScale\n" +
                        "        scale: Qt.vector3d(" +
                            transformScale.scale.x + ", " +
                            transformScale.scale.y + ", " +
                            transformScale.scale.z + ")\n" +
                        "    }\n" +
                        "\n" +
                        "    Mesh {\n" +
                        "        id: source_mesh\n" +
                        "        source: \"" + targetMesh + "\"\n" +
                        "    }\n" +
                        "\n" +
                        "    mesh: source_mesh\n" +
                        "    effect: Effect {}\n" +
                        "    transform: [\n" +
                        "        transformTranslate,\n" +
                        "        transformRotate,\n" +
                        "        transformScale\n" +
                        "    ]\n" +
                        "}\n";

                quickSave.filename = source_mesh.source
                quickSave.data = saveData
                var result = quickSave.save
                console.log("If there was an error it will be after here:" + result)
            }
        }
    }
}
