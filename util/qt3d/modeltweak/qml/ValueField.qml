import QtQuick 1.0

FocusScope {
    property alias label: textLabel.text;
    property alias value: textInput.text;
    property double delta: 0.1;
    property double max: 1;
    property double min: -1;
    property bool limit: false;

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
            horizontalAlignment: TextInput.AlignHCenter
            //FIXME: this should not be hard coded, but for some reason the display does not update properly if I use parent.width
            width: 200
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

    Keys.onUpPressed:    {
        var t = value*1 + delta; // this is sooooooo stupid!
        if (limit)
            update(t>max ? max : t);
        else
            update(t);
    }
    Keys.onDownPressed:  {
        var t = value*1 - delta; // this is sooooooo stupid too!
        if (limit)
            update(t<min ? min : t);
        else
            update(t);
    }
    Keys.onReturnPressed: { updateMe(); }
}

