import QtQuick 1.0

Rectangle {
    anchors.topMargin: 8
    border.width: 3
    height: 30
    color: "#CCC"

    property alias buttonText: text.text
    Text {
        id: text
        anchors.fill: parent
        font.pixelSize: 20
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

    }
    MouseArea {
        anchors.fill: parent
        onClicked: parent.onClicked();
        onPressed: parent.color = "#999"
        onReleased: parent.color = "#CCC"
    }
}

