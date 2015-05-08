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
import QtQuick.Scene3D 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    id: main

    property real rotationValue: 0

    Scene3D {
        id: scene3D
        width: Math.min(parent.width, parent.height) - 100
        height: width
        anchors.verticalCenter: parent.verticalCenter
        focus: true
        aspects: "input"

        Logo {
            id: watch
        }
    }

    ColumnLayout {
        id: colorLayout
        anchors.left: scene3D.right
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: scene3D.top
        spacing: 5

        Text { text: "Appearance"; font.bold: true }
        Text { text: "Ambient color RGB" }
        RowLayout {
            Text { text: "R" }
            Slider {
                id: color_r
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 255
                value: 128
            }
        }
        RowLayout {
            Text { text: "G" }
            Slider {
                id: color_g
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 255
                value: 195
            }
        }
        RowLayout {
            Text { text: "B" }
            Slider {
                id: color_b
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 255
                value: 66
            }
        }
        Text { text: "Shininess" }
        Slider {
            id: shining
            Layout.fillWidth: true
            minimumValue: 30
            maximumValue: 90
            value: 50
        }
    }

    ColumnLayout {
        id: transformLayout
        anchors.left: scene3D.right
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: colorLayout.bottom
        anchors.topMargin: 10
        spacing: 5

        Text { text: "Item transform"; font.bold: true }
        Text { text: "Rotation" }
        RowLayout {
            Text { text: "X" }
            Slider {
                id: rotation_x
                Layout.fillWidth: true
                minimumValue: 45
                maximumValue: 135
                value: rotationValue + 90
            }
        }
        RowLayout {
            Text { text: "Y" }
            Slider {
                id: rotation_y
                Layout.fillWidth: true
                minimumValue: 135
                maximumValue: 225
                value: rotationValue + 180
            }
        }
        RowLayout {
            Text { text: "Z" }
            Slider {
                id: rotation_z
                Layout.fillWidth: true
                minimumValue: -45
                maximumValue: 45
                value: rotationValue
            }
        }

        RowLayout {
            CheckBox {id: animation; text: "Animation"; checked: false}
        }
    }

    ColumnLayout {
        id: cameraLayout
        anchors.left: scene3D.right
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: transformLayout.bottom
        anchors.topMargin: 10
        spacing: 5

        Text { text: "Camera"; font.bold: true }
        Text { text: "View Center Z" }
        Slider {
            id: viewCenter_z
            Layout.fillWidth: true
            minimumValue: -12
            maximumValue: -4
            value: -7.5
        }
    }

    SequentialAnimation {
        running: true
        paused: !animation.checked
        loops: Animation.Infinite

        NumberAnimation {
            target: main
            property: "rotationValue"
            easing.type: Easing.OutQuad
            duration: 1000
            from: 0
            to: 45
        }
        NumberAnimation {
            target: main
            property: "rotationValue"
            easing.type: Easing.InOutQuad
            duration: 1000
            from: 45
            to: -45
        }
        NumberAnimation {
            target: main
            property: "rotationValue"
            easing.type: Easing.InQuad
            duration: 1000
            from: -45
            to: 0
        }
    }
}
