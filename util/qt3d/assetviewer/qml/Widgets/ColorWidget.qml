import QtQuick 1.0

Item {
    id: colorWidget
    width: parent.width; height: 16

    property variant targetColor
    property alias text: flatText.text

    Rectangle {
        height: 2
        anchors {
            left: parent.left; right: flatSet.right
            bottom: parent.bottom
            rightMargin: flatSet.width/2
        }
        color: colorWidget.targetColor.color
    }
    Text {
        id: flatText
        anchors { top: parent.top; left: parent.left }
        text:"Color"
        color: "white"
    }
    BlenderToggle {
        id: flatSet
        width: 32
        anchors { right: parent.right }
        height: parent.height
        buttonText: "Set"
        radius: 6
        onClicked: {
            colorPicker.setTarget(colorWidget.targetColor);
            colorPicker.visible = true
        }
    }
}
