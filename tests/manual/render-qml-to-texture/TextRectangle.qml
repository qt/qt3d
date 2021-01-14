/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
