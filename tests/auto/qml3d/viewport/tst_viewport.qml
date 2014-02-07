/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
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
import QtTest 1.0

Viewport {
    id: viewport

    Camera {
        id: newCamera
        eye: Qt.vector3d(0, 1, 52)
    }

    Light {
        id: newLight
        ambientColor: "#ff00ee"
    }

    LightModel {
        id: newLightModel
        ambientSceneColor: "#ff00ee"
    }

    TestCase {
        name: "Viewport"

        function test_modes() {
            verify(!viewport.picking, "picking")
            verify(!viewport.showPicking, "showPicking")
            verify(viewport.navigation, "navigation")
            verify(!viewport.blending, "blending")

            viewport.picking = true
            viewport.showPicking = true
            viewport.navigation = false
            viewport.blending = true

            verify(viewport.picking, "picking-modified")
            verify(viewport.showPicking, "showPicking-modified")
            verify(!viewport.navigation, "navigation-modified")
            verify(viewport.blending, "blending-modified")
        }

        function test_camera() {
            verify(viewport.camera != null, "camera")
            verify(viewport.camera != newCamera, "camera-obj")
            compare(viewport.camera.projectionType, 0, "projectionType")
            compare(viewport.camera.eye, Qt.vector3d(0, 0, 10), "eye")
            compare(viewport.camera.center, Qt.vector3d(0, 0, 0), "center")
            compare(viewport.camera.upVector, Qt.vector3d(0, 1, 0), "upVector")
            compare(viewport.camera.fieldOfView, 0, "fieldOfView")
            compare(viewport.camera.nearPlane, 5, "nearPlane")
            compare(viewport.camera.farPlane, 1000, "farPlane")
            compare(viewport.camera.eyeSeparation, 0, "eyeSeparation")
            compare(viewport.camera.adjustForAspectRatio, true, "adjustForAspectRatio")

            var oldCamera = viewport.camera

            viewport.camera = newCamera
            verify(viewport.camera == newCamera, "newCamera")
            compare(viewport.camera.eye, Qt.vector3d(0, 1, 52), "eye-new")

            compare(oldCamera.eye, Qt.vector3d(0, 0, 10), "eye-old")
        }

        function test_light() {
            verify(viewport.light != null, "light")
            verify(viewport.light != newLight, "light-obj")
            compare(viewport.light.type, 0, "type-directional")
            compare(viewport.light.direction, Qt.vector3d(0, 0, 1), "direction")
            compare(viewport.light.ambientColor, "#000000", "ambientColor")
            compare(viewport.light.diffuseColor, "#ffffff", "diffuseColor")
            compare(viewport.light.specularColor, "#ffffff", "specularColor")
            compare(viewport.light.spotDirection, Qt.vector3d(0, 0, -1), "spotDirection")
            compare(viewport.light.spotExponent, 0, "spotExponent")
            compare(viewport.light.spotAngle, 180, "spotAngle")
            compare(viewport.light.constantAttenuation, 1, "constantAttenuation")
            compare(viewport.light.linearAttenuation, 0, "linearAttenuation")
            compare(viewport.light.quadraticAttenuation, 0, "quadraticAttenuation")

            var oldLight = viewport.light

            viewport.light = newLight
            verify(viewport.light == newLight, "newLight")
            compare(viewport.light.ambientColor, "#ff00ee", "ambientColor-new")

            compare(oldLight.ambientColor, "#000000", "ambientColor-old")
        }

        function test_lightModel() {
            verify(viewport.lightModel == null, "lightModel")

            viewport.lightModel = newLightModel
            verify(viewport.lightModel == newLightModel, "newLightModel")
            compare(viewport.lightModel.ambientSceneColor, "#ff00ee", "ambientSceneColor")
        }
    }
}
