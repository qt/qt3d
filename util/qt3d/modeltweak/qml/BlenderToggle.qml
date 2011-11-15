import QtQuick 1.0

Rectangle {
    anchors.topMargin: 8
    border.width: 1
    border.color: "#191919"
    radius: 8
    height: 20
    color: "#999999"

    property alias buttonText: text.text
    Text {
        id: text
        anchors.fill: parent
        font.pixelSize: 12
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

    }
    MouseArea {
        anchors.fill: parent
        onClicked: parent.onClicked();
        onPressed: parent.color = "#646464"
        onReleased: parent.color = "#999999"
        hoverEnabled: true
        onEntered: parent.color = "#BEBEBE"
        onExited: parent.color = "#999999"
    }
}

