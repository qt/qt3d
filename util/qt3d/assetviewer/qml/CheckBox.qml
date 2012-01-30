import QtQuick 1.0

Item {
    id: checkBox
    height: Math.max (textBox.height, check.height)
    width: textBox.width + check.width
    property alias text: textBox.text
    property bool checked: false
    signal clicked(bool checked)
    property variant color: Qt.rgba(1,1,1,1)

    Text {
        id: textBox
        anchors.left: parent.left
        color: checkBox.color
    }

    Rectangle {
        id: check
        width: 16
        height: 16
        anchors.right: parent.right

        border.color: checkBox.color
        border.width: 1
        color: Qt.rgba(0,0,0,0)

        Rectangle {
            anchors.centerIn: parent

            border.color: checkBox.color
            border.width: 1

            width: parent.width - 6
            height: parent.height - 6
            color: checkBox.color
            opacity: checked
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: { checked = !checked; checkBox.clicked(checked) }
    }
}
