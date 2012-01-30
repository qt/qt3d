import QtQuick 1.0
import Qt3D 1.0
import AssetViewer 1.0
import "Widgets"
import "ColorUtils.js" as ColorUtils

Row {
    id: properties
    spacing: 30
    property alias rotateLocked: imageR.isLocked
    property alias scaleLocked: imageS.isLocked
    property alias translateLocked: imageP.isLocked;

    signal changed;
    Column {
        width: posX.width
        height: parent.height
        spacing: 4

        // POSITION
        Item {
            id: positionPanel
            width: parent.width
            height: imageP.height

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 8
                text: "Position";
                color: "#FFFFFF";
                font.bold: true;
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
            function update (f) {
                transformTranslate.translate = Qt.vector3d(f, transformTranslate.translate.y, transformTranslate.translate.z);
            }
            onNext: { updateMe(); focus = false; posY.focus = true; }
            onPrev: { updateMe(); focus = false; scaleZ.focus = true; }
            onFail: { imageP.bounce=true; }
            onChanged: { properties.changed(); }
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
            onChanged: { properties.changed(); }
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
            onChanged: { properties.changed(); }
        }

        // ROTATE
        Item {
            id: rotationPanel
            width: parent.width
            height: imageR.height
            property bool dirty: false
            Text {
                anchors.left: parent.left
                anchors.leftMargin: 8
                text: "Rotation";
                color: "#FFFFFF"
                font.bold: true
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
            onChanged: { properties.changed(); }
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
            onChanged: { properties.changed(); }
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
            onChanged: { properties.changed(); }
        }

        // SCALE
        Item {
            id: scalePanel
            width: parent.width
            height: imageS.height
            property bool dirty: false

            Item {
                width: parent.width
                height: scaleText.height
                Text {
                    id: scaleText
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    text: "Scale";
                    color: "#FFFFFF"
                    font.bold: true
                }
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
            onChanged: { properties.changed(); }
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
            onChanged: { properties.changed();    }
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
            onChanged: { properties.changed(); }
        }
    }

    // Material, Effect
    Column {
        width: 150
        spacing: 5
        Item {
            id: effectPanel
            width: parent.width
            height: imageS.height
            property bool dirty: false

            Item {
                width: parent.width
                height: effectText.height
                Text {
                    id: effectText
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    text: "Effect";
                    color: "#FFFFFF"
                    font.bold: true
                }
            }
        }


        CheckBox {
            id: fromMesh
            width: parent.width
            text: "From Mesh"
            checked: true
            onClicked: {
                useCustomEffect = !checked;
                if (checked) {
                    // A horrible hack to reload the default mesh material
                    // TODO: Find a better way of doing this (i.e. no runtime errors)
                    source_mesh.source = "";
                    source_mesh.source = targetMesh;
                }
            }
        }

        Column {
            enabled: !fromMesh.checked
            opacity: enabled ? 1.0: 0.5
            width: parent.width; spacing: 5

            move: Transition {
                NumberAnimation {
                    properties: "y"
                    easing.type: Easing.InOutQuad;
                    duration: 150
                }
            }

            Behavior on opacity {
                NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }
            }

            CheckBox {
                width: parent.width
                text: "Decal"
                checked: false
                onClicked: {
                    modelEffect.decal = checked
                }
            }

            Item {
                width: parent.width
                height: colorText.height
                Text {
                    id: colorText
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    text: "- Color";
                    color: "#FFFFFF"
                    font.bold: true
                }
            }

            CheckBox {
                id: simpleColor
                width: parent.width
                text: "Simple"
                checked: true
                onClicked: {
                    useCustomMaterial = !checked;
                }
            }

            ColorWidget {
                id: flat
                visible: opacity != 0
                opacity: simpleColor.checked
                width: parent.width; height: 16
                targetColor: modelEffect.hsv
                text: "Flat Color"
                Behavior on opacity {
                    NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }
                }
            }

            ColorWidget {
                id: ambient
                visible: opacity != 0
                opacity: !flat.visible
                width: parent.width; height: 16
                targetColor: modelMaterial.amb_hsv
                text: "Ambient Color"
                Behavior on opacity {
                    NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }
                }
            }

            ColorWidget {
                id: diffuse
                visible: opacity != 0
                opacity: !flat.visible
                width: parent.width; height: 16
                targetColor: modelMaterial.dif_hsv
                text: "Diffuse Color"
                Behavior on opacity {
                    NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }
                }
            }

            ColorWidget {
                id: specular
                visible: opacity != 0
                opacity: !flat.visible
                width: parent.width; height: 16
                targetColor: modelMaterial.spec_hsv
                text: "Specular Color"
                Behavior on opacity {
                    NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }
                }
            }

            Item { width: parent.width; height: 5 }

            Item {
                visible: opacity != 0
                opacity: !flat.visible
                height: shineText.height + shinyValue.height + 5
                width: parent.width

                Behavior on opacity {
                    NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }
                }

                Text {
                    id: shineText
                    text: "Shininess:"
                    color: "white"
                }
                InputBox {
                    anchors.right: parent.right
                    width: parent.width - shineText.width - 5
                    height: shineText.height
                    input.text: modelMaterial.shininess
                }
                Rectangle {
                    width: parent.width - 8; height: 1
                    anchors.centerIn: shinyValue
                    color: "gray"
                }

                SliderHandle {
                    id: shinyValue
                    width: parent.width; height: 10
                    anchors { top: shineText.bottom; topMargin: 5 }
                    horizontal: true
                    onValueChanged: {
                        modelMaterial.shininess = Math.round(value * 128)
                    }
                }
            }

            Item { width: parent.width; height: 5 }

            Item {
                width: parent.width
                height: textureText.height
                Text {
                    id: textureText
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    text: "- Texture";
                    color: "#FFFFFF"
                    font.bold: true
                }
            }

            Item {
                width: parent.width
                height: texturePreview.height
                Image {
                    id: texturePreview
                    width: 60; height: width
                    source: textureFile.filename
                }
                Rectangle {
                    anchors.fill: texturePreview
                    border.color: "white"
                    border.width: 1
                    color: "transparent"
                }

                BlenderToggle {
                    width: 60; height: 20
                    anchors.right: parent.right
                    buttonText: "Open..."
                    onClicked: {
                        textureFile.load();
                    }
                }
            }
        }
    }
}
