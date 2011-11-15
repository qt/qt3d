import QtQuick 1.0

Item {
    id: valueSlider

    property string label: "";
    property alias value: textInput.text;
    property double delta: 0.1;
    property double max: 1;
    property double min: -1;
    property bool limitMax: false;
    property bool limitMin: false;

    width: rect.width
    height: 20

    property int textWidth: 80

    Rectangle {
        id: rect
        anchors.right: parent.right
        radius: 8
        border.width: 1
        border.color: "#191919"
        color: "#B4B4B4"
        width: radius/2 + valueSlider.textWidth + plus.width + minus.width
        height: parent.height


        Text {
            id: textLabel
            color: "black"
            width: textInput.width
            height: textInput.height
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: valueSlider.label + " " + textInput.text
            visible: true
        }

        TextInput {
            id: textInput
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: TextInput.AlignHCenter
            width: valueSlider.textWidth
            validator: DoubleValidator{}
            onAccepted: {
                parent.parent.update(text)
                focus = false
            }
            visible: false
            onFocusChanged: {
                if(focus == false) {
                    visible = false
                    textLabel.visible = true
                }
                else {
                    visible = true
                    textLabel.visible = false
                }
            }
        }
        Image {
            id: plus
            source: "plus.png"
            anchors.left: textInput.right
            anchors.verticalCenter: parent.verticalCenter
        }
        Image {
            id: minus
            source: "minus.png"
            anchors.right: textInput.left
            anchors.verticalCenter: parent.verticalCenter
        }
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onEntered: parent.color = "#999999"
            onExited: parent.color = "#B4B4B4"
            hoverEnabled: true
            onClicked: {
                if(mouse.x<width/3)
                    decDelta();
                else if(mouse.x>width/3*2)
                    incDelta();
                else {
                    textLabel.visible = false;
                    textInput.visible = true;
                    textInput.focus = true;
                    textInput.selectAll();
                }

            }
            onPressed: parent.color = "#808080"
            onReleased: parent.color = "#999999"
        }
    }

    Timer {
        interval: 100;
        running: mouseArea.pressedButtons & Qt.LeftButton && (mouseArea.mouseX > valueSlider.width/3*2)
        repeat: true
        onTriggered: incDelta()
    }

    Timer {
        interval: 100;
        running: mouseArea.pressedButtons & Qt.LeftButton && (mouseArea.mouseX < valueSlider.width/3)
        repeat: true
        onTriggered: decDelta()
    }

    function updateMe() {
        update(textInput.text)
    }

    function incDelta() {
        var t = value*1 + delta;
        if (limitMax)
            update(t>max ? max : t);
        else
            update(t);
        console.log("updated! " + t);

    }

    function decDelta() {
        var t = value*1 - delta;
        if (limitMin)
            update(t<min ? min : t);
        else
            update(t);
        console.log("updated! " + t);
    }

    Keys.onUpPressed: incDelta()
    Keys.onDownPressed: decDelta()
    Keys.onReturnPressed: updateMe()
}
