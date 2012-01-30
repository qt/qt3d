import QtQuick 1.0

Item {
    id: textInput
    width: 60
    height: 20
    property alias label: lbl.text
    property alias input: inputBox

//    Binding {
//        target: textInput; property: "value"
//        value: inputBox.text
//    }
//    Binding {
//        target: inputBox; property: "text"
//        value: { var result = textInput.value*100; return Math.round(result)/100 }
//    }

    Text {
        id: lbl
        color: "lightgray"
        anchors.left: parent.left
    }

    Rectangle {
        border.color: "gray"
        color: "transparent"
        width: 40
        height: inputBox.height
        anchors.right: parent.right
        TextInput {
            id: inputBox
            anchors { fill: parent; leftMargin: 5 }
            color: "white"
            text: { var result = textInput.value*100; return Math.round(result)/100 }
            selectByMouse: true; readOnly: true
        }
    }
}
