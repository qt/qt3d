import QtQuick 1.0

Rectangle {
    id: button
    color: mouseArea.pressed ? "#000000" : "#ffffff"
    border.width: 1
    anchors.margins: 5
    radius: 5
    property alias text : text.text
    signal clicked;
    Text {
        id: text
        color: mouseArea.pressed ? "#ffffff" : "#000000";
        anchors.centerIn: parent
        text: qsTr("A Button")
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: button.clicked()

    }
}
