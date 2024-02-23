// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0

Rectangle {
    id: rectangle
    property string myText: "The quick brown fox jumps over the lazy dog."
    x: 352
    y: 272
    width: 320
    height: 480
    color: "steelblue"

    FontLoader { id: fixedFont; name: "Courier" }

    Column {
        anchors { fill: parent; leftMargin: 10; rightMargin: 10; topMargin: 10 }
        spacing: 15

        Text {
            text: rectangle.myText
            color: "lightsteelblue"
            width: parent.width
            wrapMode: Text.WordWrap
            font.family: "Times"
            font.pixelSize: 20
        }
        Text {
            text: rectangle.myText
            color: "lightsteelblue"
            width: parent.width
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            font { family: "Times"; pixelSize: 20; capitalization: Font.AllUppercase }
        }
        Text {
            text: rectangle.myText
            color: "lightsteelblue"
            width: parent.width
            horizontalAlignment: Text.AlignRight
            wrapMode: Text.WordWrap
            font { family: fixedFont.name; pixelSize: 20; weight: Font.Bold; capitalization: Font.AllLowercase }
        }
        Text {
            text: rectangle.myText
            color: "lightsteelblue"
            width: parent.width
            wrapMode: Text.WordWrap
            font { family: fixedFont.name; pixelSize: 20; italic: true; capitalization: Font.SmallCaps }
        }
    }
}
