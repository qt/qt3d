import QtQuick 1.0
import "Widgets"
import "ColorUtils.js" as ColorUtils

Rectangle {
    id: picker
    property alias alpha: alphaSlider.value
    property alias hue: hueSlider.value
    property alias sat: colorSelector.sat
    property alias val: colorSelector.val
    property variant colorSelect: Qt.hsla(hsl_hue, hsl_sat, hsl_lum, alpha)
    property HSVColor targetColor

    HSVColor { id: oldColor }

    property real hsl_hue: hue
    property real hsl_sat: sat*val*((hsl_lum <= 2) ? hsl_lum*2 : 2*(1 - hsl_lum))
    property real hsl_lum: ((2.0 - sat)*val)/2

    property int red: rgb[0]
    property int green: rgb[1]
    property int blue: rgb[2]

    property variant rgb: ColorUtils.hsvToRgb(hue, sat, val)

    onRgbChanged: updateTarget()
    onAlphaChanged: updateTarget()

    anchors.fill: parent
    property real bgAlpha: 0.5
    color: Qt.rgba(1,1,1,bgAlpha)

    function apply() {
        updateTarget();
        picker.visible = false;
    }

    function cancel() {
        resetTarget();
        picker.visible = false;
    }

    function setTarget(target) {
        targetColor = target;
        hue = target.hue;
        sat = target.sat;
        val = target.val;
        alpha = target.alpha;
        oldColor.hue = target.hue;
        oldColor.sat = target.sat;
        oldColor.val = target.val;
        oldColor.alpha = target.alpha
    }

    function updateTarget() {
        if (targetColor === null || !picker.visible) return;
        targetColor.hue = hue;
        targetColor.sat = sat;
        targetColor.val = val;
        targetColor.alpha = alpha;
    }

    function resetTarget() {
        if (targetColor === null) return;
        targetColor.hue = oldColor.hue;
        targetColor.sat = oldColor.sat;
        targetColor.val = oldColor.val;
        targetColor.alpha = oldColor.alpha;
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {}
    }

    Rectangle {
        width: pickerLayout.implicitWidth+20
        height: 250
        anchors.centerIn: parent

        border.width: 2
        border.color: "white"
        color: mainwindow.color

        Row {
            id: pickerLayout
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
                margins: 10
            }
            height: 200
            spacing: 10

            Item {
                id: colorViewer
                width: height
                height: parent.height
                Rectangle {
                    anchors.fill: parent
                    rotation: -90
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: Qt.hsla(hueSlider.value, 1.0, 1.0) }
                        GradientStop { position: 1.0; color: Qt.hsla(hueSlider.value, 1.0, 0.5) }
                    }
                }
                Rectangle {
                    anchors.fill: parent
                    border.color: "lightgray"
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: Qt.rgba(0,0,0,0) }
                        GradientStop { position: 1.0; color: Qt.rgba(0,0,0,1) }
                    }
                }

                ColorSelector {
                    id: colorSelector
                    anchors.fill: parent
                }
            }

            Item {
                width: 20
                height: parent.height
                Rectangle {
                    anchors.fill: parent
                    border.color: "lightgray"
                    gradient: Gradient {
                        GradientStop { position: 1.0;  color: Qt.hsla(1.0, 1.0, 0.5) }
                        GradientStop { position: 0.85; color: Qt.hsla(0.85, 1.0, 0.5) }
                        GradientStop { position: 0.76; color: Qt.hsla(0.76, 1.0, 0.5) }
                        GradientStop { position: 0.5;  color: Qt.hsla(0.5, 1.0, 0.5) }
                        GradientStop { position: 0.33; color: Qt.hsla(0.33, 1.0, 0.5) }
                        GradientStop { position: 0.16; color: Qt.hsla(0.16, 1.0, 0.5) }
                        GradientStop { position: 0.0;  color: Qt.hsla(0.0, 1.0, 0.5) }
                    }
                }

                SliderHandle {
                    id: hueSlider
                    anchors.fill: parent
                }
            }

            Item {
                width: 20
                height: parent.height

                Checkered {
                    anchors.fill: parent
                }

                Rectangle {
                    anchors.fill: parent
                    border.color: "lightgray"
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0;
                            color: Qt.hsla(picker.hsl_hue, picker.hsl_sat, picker.hsl_lum)
                        }
                        GradientStop {
                            position: 1.0;
                            color: Qt.hsla(picker.hsl_hue, picker.hsl_sat, picker.hsl_lum, 0)
                        }
                    }
                }

                SliderHandle {
                    id: alphaSlider
                    anchors.fill: parent
                    inverted: true
                }
            }

            Item {
                width: 60
                height: parent.height

                Item {
                    id: preview
                    width: parent.width; height: 40
                    Checkered {
                        anchors.fill: parent
                    }
                    Rectangle {
                        anchors.fill: parent
                        border.color: "lightgray"
                        color: picker.colorSelect
                    }
                }

                Column {
                    anchors.top: preview.bottom
                    anchors.topMargin: 5
                    spacing: 3
                    width: parent.width

                    InputBox {
                        id: redIn
                        width: parent.width; height: 20
                        label: "R:"
                    }

                    InputBox {
                        id: greenIn
                        width: parent.width; height: 20
                        label: "G:"
                    }

                    InputBox {
                        id: blueIn
                        width: parent.width; height: 20
                        label: "B:"
                    }

                    Binding {
                        target: redIn.input; property: "text"
                        value: Math.round(rgb[0])
                    }
                    Binding {
                        target: greenIn.input; property: "text"
                        value: Math.round(rgb[1])
                    }
                    Binding {
                        target: blueIn.input; property: "text"
                        value: Math.round(rgb[2])
                    }

                    Item {
                        width: 5; height: 5
                    }

                    InputBox {
                        id: hueIn
                        width: parent.width; height: 20
                        label: "H:"
                    }

                    InputBox {
                        id: satIn
                        width: parent.width; height: 20
                        label: "S:"
                    }

                    InputBox {
                        id: valIn
                        width: parent.width; height: 20
                        label: "V:"
                    }

                    Binding {
                        target: hueIn.input; property: "text"
                        value: Math.round(picker.hue*100)/100
                    }
                    Binding {
                        target: satIn.input; property: "text"
                        value: Math.round(picker.sat*100)/100
                    }
                    Binding {
                        target: valIn.input; property: "text"
                        value: Math.round(picker.val*100)/100
                    }
                }
            }
        }

        Row {
            anchors {
                margins: 5
                top: pickerLayout.bottom
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 10
            BlenderToggle {
                width: 80
                buttonText: "Apply"
                onClicked: picker.apply()
            }
            BlenderToggle {
                width: 80
                buttonText: "Cancel"
                onClicked: picker.cancel()
            }
        }
    }
}
