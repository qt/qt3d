import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0

Rectangle {
    anchors.topMargin: 15
    anchors.leftMargin: 15
    border.width: 1
    border.color: "#191919"
    radius: 8
    height: 20
    color: "#999999"

    property alias buttonText: text.text
    property alias imageSrc: img.source

    signal clicked


    Image {
        id: img
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        id: text
        anchors.fill: parent
        font.pixelSize: 12
        anchors.left: img.left
        anchors.leftMargin: 5
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        anchors.fill: parent
        onClicked: parent.clicked()
        onPressed: parent.color = "#646464"
        onReleased: parent.color = "#999999"
        hoverEnabled: true
        onEntered: parent.color = "#BEBEBE"
        onExited: parent.color = "#999999"
    }
}

