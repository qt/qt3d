/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.1 as QQ2

Entity {
    id: root
    property real hue: 0.0
    property alias animateColors: hueAnim.running

    QQ2.NumberAnimation {
        id: hueAnim
        target: root
        property: "hue"
        from: 0.0; to: 1.0
        duration: 200000
        running: false
        loops: QQ2.Animation.Infinite
    }

    Entity {
        id: _private
        property color color1: Qt.hsla( (hue + 0.59) % 1, 0.53, 0.59 )
        property color color2: Qt.hsla( (hue + 0.59) % 1, 1.0, 0.15 )
    }

    components: [
        Layer {
            names: "background"
        },

        PlaneMesh {
            width: 2.0
            height: 2.0
            meshResolution: Qt.size( 2, 2 )
        },

        Transform {
            // Rotate the plane so that it faces us
            Rotate {
                axis: Qt.vector3d( 1.0, 0.0, 0.0 )
                angle: 90
            }
        },

        Material {
            effect: BackgroundEffect {}
            parameters: [
                Parameter { name: "color1"; value: Qt.vector3d( _private.color1.r, _private.color1.g, _private.color1.b ) },
                Parameter { name: "color2"; value: Qt.vector3d( _private.color2.r, _private.color2.g, _private.color2.b ) }
            ]
        }
    ]

}
