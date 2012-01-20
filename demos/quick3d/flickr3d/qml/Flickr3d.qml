/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Viewport {
    width: parent.width
    height: parent.height
    renderMode: "DirectRender"

    camera: Camera {
        id: cam
        property real viewAngle: 0.0
        eye: Qt.vector3d(Math.sin(cam.viewAngle) * 20.0, 3.0, Math.cos(cam.viewAngle) * 20.0)
    }

    NumberAnimation {
        running: true
        target: cam; property: "viewAngle";
        from: 0.0; to: 360.0; duration: 2000000;
        loops: Animation.Infinite
    }

    Skybox {
        source: "skybox"
    }

    Cube {
        effect: Effect {
            color: "#aaca00"
            texture: "qtlogo.png"
            decal: true
        }
    }
}
