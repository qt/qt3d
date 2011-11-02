import QtQuick 1.0

FocusScope {
    property alias label: textLabel.text;
    property alias value: textInput.text;
    property double delta: 0.1;

    width: parent.width
    height: childrenRect.height

    Text {
        id: textLabel
        anchors.left: parent.left
        color: "#FFFFFF"
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width/3
        visible: parent.label !== ""
    }
    Rectangle {
        anchors.right: parent.right
        radius: 8
        border.width: 2
        border.color: "black"
        color: "white"
        width: parent.label !== "" ? parent.width/3*2 : parent.width
        height: childrenRect.height

        TextInput {
            anchors.horizontalCenter: parent.horizontalCenter
            id: textInput
            focus: true
            validator: DoubleValidator{}
            onAccepted: {
                console.log("Accept!")
                parent.parent.update(text)

            }

        }
    }
    function updateMe() {
        update(textInput.text)
    }

    Keys.onUpPressed:    { updateInc(delta); }
    Keys.onDownPressed:  { updateInc(-delta); }
    Keys.onReturnPressed: { updateMe(); }
}

