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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3

Rectangle {
    width: 1024
    height: 1024
    color: "white"
    focus: true

    Rectangle {
        id: rect
        x: 0
        y: 0
        width: 200
        height: 200
        color: "red"
    }

    Rectangle {
        id: rect2
        width: 300
        height: 100
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "blue"
    }

    Button {
        id: button
        width: 100
        height: 80
        anchors.top: rect.bottom
        anchors.left: parent.left
        text: "button"
        scale: 2.0
    }
    CheckBox {
        id: checkbox
        width: 200
        height: 180
        scale: 2.0
        text: "checkbox"
        checked: true
        anchors.top: parent.top
        anchors.right: parent.right
    }
    Slider {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: 400
        value: 1
        minimumValue: 0
        maximumValue: 1

        onValueChanged: {
            rect.color = Qt.rgba(value, 0, 0, 1)
            rect2.color = Qt.rgba(0, 0, value, 1)
        }
    }
}
