/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

Item {
    id: fpsDisplayControl
    property bool hidden: true
    property real fps: 0.0

    onHiddenChanged: {
        if (fpsDisplayControl.hidden)
            fpsDisplay.color = "transparent"
        else
            fpsDisplay.color = "#000000FF"
    }

    onFpsChanged: {
        fpsDisplay.updateFps()
    }

    Rectangle {
        anchors.fill: parent
        id: fpsDisplay
        color: "transparent"

        property real maxFps: 60.0
        property color maxFpsColor: "#5500FF00"
        property color minFpsColor: "#55FF0000"

        function updateFps() {
            var scale = (fps > maxFps)?1.0:(fps/maxFps)
            var r = (1 - scale) * minFpsColor.r + scale * maxFpsColor.r
            var g = (1 - scale) * minFpsColor.g + scale * maxFpsColor.g
            var b = (1 - scale) * minFpsColor.b + scale * maxFpsColor.b
            var a = (1 - scale) * minFpsColor.a + scale * maxFpsColor.a
            fpsCauge.height = scale * fpsDisplay.height
            fpsCauge.color = Qt.rgba(r, g, b, a)
        }

        Rectangle {
            id: fpsCauge
            width: parent.width
            anchors.bottom: parent.bottom
            visible: !fpsDisplayControl.hidden
        }

        Text {
            id: fpsText
            text: ""+(fps | 0)
            font.family: "Helvetica"
            font.pixelSize: 16
            font.weight: Font.Light
            color: "white"
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            visible: !fpsDisplayControl.hidden
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            fpsDisplayControl.hidden = !fpsDisplayControl.hidden
        }
    }
}

