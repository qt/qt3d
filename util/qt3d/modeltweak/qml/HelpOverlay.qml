import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0

Rectangle {
    anchors.fill: parent
    color: "#00000000"

    Rectangle {
        id: helpScreen
        width: parent.width/2
        height: parent.height/2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        border.width: 4
        radius: 4
        color: "lightyellow"

        Text {
            id: header
            font { pixelSize: 32; italic: true }
            text: "Help!"
            anchors.top: parent.top
            anchors.topMargin: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Text {
            anchors.top: header.bottom
            anchors.topMargin: 16
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.right: parent.right
            anchors.rightMargin: 16
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
            text: "Apparently you're having problems." +
                  "<ul><li>Clicking on one of the panes with the <b>Left Mouse Button</b> and dragging with translate the position.</li>" +
                  "<li>Clicking on one of the panes with the <b>Right Mouse Button</b> and dragging will rotate the position.</li>" +
                  "<li>Clicking on one of the panes with the <b>Middle Mouse Button</b> and dragging will resize the model."
        }

        Text {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            anchors.horizontalCenter: parent.horizontalCenter
            text: "[ Click to Close ]"
        }
    }
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        onClicked: parent.visible = false
        //onPressed:
        //onReleased:
    }
}

