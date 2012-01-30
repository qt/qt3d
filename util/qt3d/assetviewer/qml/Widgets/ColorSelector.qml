import QtQuick 1.0

Item {
    id: select
    property real sat: 1.0
    property real val: 1.0
    width: parent.width
    height: parent.height

    Binding {
        target: handle; property: "x"
        value: select.sat * select.width
    }
    Binding {
        target: select; property: "sat"
        value: handle.x/width
    }

    Binding {
        target: handle; property: "y"
        value: (1.0 - select.val) * select.height
    }
    Binding {
        target: select; property: "val"
        value: 1.0 - handle.y/height
    }

    Item {
        id: handle
        width: 20
        height: width
        x: 0.0; y: 0.0

        Rectangle {
            x: -width/2
            y: x
            width: parent.width-2
            height: width
            radius: width/2

            border.color: "white"
            color: "transparent"
        }
        Rectangle {
            x: -width/2
            y: x
            width: parent.width
            height: width
            radius: width/2

            border.width: 2
            border.color: "black"
            color: "transparent"
        }
    }

    MouseArea {
        anchors.fill: parent
        function mouseEvent(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                handle.x = Math.max(0, Math.min(height, mouse.x))
                handle.y = Math.max(0, Math.min(height, mouse.y))
            }
        }
        onPressed: mouseEvent(mouse);
        onPositionChanged: mouseEvent(mouse);
        hoverEnabled: true
        onEntered: handle.opacity = 0.5;
        onExited: { if (!pressed) handle.opacity = 1.0 }
        onReleased: { if (!containsMouse) handle.opacity = 1.0 }
    }
}
