import QtQuick 1.0

Item {
    id: slider
    width: parent.width
    height: parent.height
    property real value: 0.0
    property bool inverted: false
    property bool horizontal: false

    signal changed;

    Binding {
        target: slider; property: "value"
        value: {
            var res;
            var pos = horizontal ? handle.x : handle.y;
            var delta = horizontal ? width-handle.width : height-handle.height;
            res = pos/delta;
            return inverted ? 1-res : res
        }
    }
    Binding {
        target: handle; property: "y"
        value: {
            var res = inverted ? 1 - slider.value : slider.value
            if (!horizontal) return res*(height-handle.height);
        }
    }
    Binding {
        target: handle; property: "x"
        value: {
            var res = inverted ? 1 - slider.value : slider.value
            if (horizontal) return res*(width-handle.width);
        }
    }

    Rectangle {
        id: handle
        width: 8 + (horizontal ? 0 : parent.width)
        height: 8 + (!horizontal ? 0 : parent.height)
        anchors.horizontalCenter: horizontal ? undefined : parent.horizontalCenter
        anchors.verticalCenter: !horizontal ? undefined : parent.verticalCenter
        radius: 2
        Behavior on opacity {
            NumberAnimation { easing.type: Easing.InOutQuad; duration: 80 }
        }
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FFFFFF" }
            GradientStop { position: 1.0; color: "#FFFFFF" }
        }
        border.width: 1
        border.color: "darkgray"
        onXChanged: slider.changed()
        onYChanged: slider.changed()
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        function mouseEvent(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                if (horizontal)
                    handle.x = Math.max(0, Math.min(width-handle.width, mouse.x-handle.width/2))
                else
                    handle.y = Math.max(0, Math.min(height-handle.height, mouse.y-handle.height/2));
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
