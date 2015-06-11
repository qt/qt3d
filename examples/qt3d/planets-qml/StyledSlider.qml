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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3

Slider {
    id: styledSlider

    style: SliderStyle {
        groove: Rectangle {
            implicitWidth: 200
            implicitHeight: 5
            color: "#5d5b59"
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: styleData.handlePosition
                height: 5
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#80c342" }
                    GradientStop { position: 1.0; color: "#328930" }
                }
            }
        }
        handle: Rectangle {
            Rectangle {
                anchors.centerIn: parent
                color: "#aeadac"
                border.color: "#1e1b18"
                border.width: 1
                implicitWidth: 8
                implicitHeight: 20
            }
            anchors.centerIn: parent
            color: "transparent"
            border.width: 0
            implicitWidth: 50
            implicitHeight: 50
        }
    }
}

