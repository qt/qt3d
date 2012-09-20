/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Shapes 2.0

Viewport {
    width: parent.width
    height: parent.height

    camera: Camera {
        id: cam
        property real viewAngle: 0.0
        eye: Qt.vector3d(Math.sin(cam.viewAngle) * 20.0, 3.0, Math.cos(cam.viewAngle) * 20.0)
    }

    NumberAnimation {
        running: Qt.application.active
        target: cam; property: "viewAngle";
        from: 0.0; to: 360.0; duration: 2000000;
        loops: Animation.Infinite
    }

    RssModel { id: rssModel }

    Item3D {
        transform: [
            Rotation3D { axis: Qt.vector3d(1, 0, 0); angle: 90 },
            Translation3D { translate: Qt.vector3d(0, 1, 0) }
        ]

        Cylinder {
            levelOfDetail: 1
            length: 2.0
            radius: 2.8
            effect: Effect {
                color: "#ccccdd"
            }
        }
    }

    Skybox {
        source: "./"
    }

    Component {
        id: octoDisplayDelegate
        Item3D {
            transform: [
                Translation3D { translate: Qt.vector3d(0, 0, 2.6) },
                // index is a special variable that comes from model instancing
                Rotation3D { axis: Qt.vector3d(0, 1, 0); angle: (360 / 16) * (index * 2 + 1) }
            ]
            Item3D {
                enabled: index > -1 && index < 9
                transform: [
                    Rotation3D { axis: Qt.vector3d(1, 0, 0); angle: 90 }
                ]
                Quad {
                    effect: Effect {
                        //The current texture downloading does not operate as network URLs are unsupported
                        //in qt 5.
                        //texture: model.imagePath
                        texture: "qtlogo.png"
                        blending: true
                    }
                }
            }
        }
    }

    Repeater {
        delegate: octoDisplayDelegate
        model: rssModel
    }
}

