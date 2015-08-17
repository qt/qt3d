/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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
import QtQuick 2.4 as QQ2

Entity {
    id: root
    property ShaderData sectionData: ShaderData {
        property real sectionsCount: 3
        property ShaderDataArray sections: ShaderDataArray {
            ShaderData {
                property vector4d equation: clipPlane0.equation
                property vector3d center: clipPlane0.center
            }
            ShaderData {
                property vector4d equation: clipPlane1.equation
                property vector3d center: clipPlane1.center
            }
            ShaderData {
                property vector4d equation: clipPlane2.equation
                property vector3d center: clipPlane2.center
            }
        }
    }

    Entity {
        property Layer layer: Layer {
            names: "caps"
        }

        property Material cappingMaterial: Material {
            effect: CappingMaterialEffect {
                sectionsData: root.sectionData
            }
        }

        property PlaneMesh mesh: PlaneMesh {
            width: 20.0
            height: 20.0
            meshResolution: Qt.size(2, 2)
        }

        components: [cappingMaterial, mesh, layer]
    }

    PlaneVisualizationMaterial {
        id: clipPlanesMaterial
        alpha: 0.5
    }

    // XZ
    ClipPlaneEntity {
        id: clipPlane0
        visualMaterial: clipPlanesMaterial
        center: Qt.vector3d(0, -10, 0)
        normal: Qt.vector3d(0, -1.0, 0)
    }
    // XY
    ClipPlaneEntity {
        id: clipPlane1
        visualMaterial: clipPlanesMaterial
        center: Qt.vector3d(0, 0, 10)
        normal: Qt.vector3d(0, 0, -1.0)
        rotateAxis: Qt.vector3d(1.0, 0.0, 0.0)
        rotateAngle: 90
    }

    // YZ
    ClipPlaneEntity {
        id: clipPlane2
        visualMaterial: clipPlanesMaterial
        center: Qt.vector3d(-10, 0, 0)
        normal: Qt.vector3d(1.0, 0, 0)
        rotateAxis: Qt.vector3d(0.0, 0.0, 1.0)
        rotateAngle: -90
    }

    QQ2.SequentialAnimation {
        running: true
        loops: QQ2.Animation.Infinite
        QQ2.NumberAnimation {
            target: clipPlane0
            property: "center.y"
            from: 10
            to: 0
            duration: 2000
            easing.type: QQ2.Easing.InOutQuart
        }

        QQ2.NumberAnimation {
            target: clipPlane1
            property: "center.z"
            from: 10
            to: 0
            duration: 2000
            easing.type: QQ2.Easing.InOutQuart
        }
        QQ2.NumberAnimation {
            target: clipPlane2
            property: "center.x"
            from: -10
            to: 0
            duration: 4000
            easing.type: QQ2.Easing.InOutQuart
        }
        QQ2.NumberAnimation {
            target: clipPlane1
            property: "center.z"
            from: 0
            to: 10
            duration: 4000
            easing.type: QQ2.Easing.InOutQuint
        }
        QQ2.NumberAnimation {
            target: clipPlane0
            property: "center.y"
            from: 0
            to: 10
            duration: 2000
            easing.type: QQ2.Easing.InOutQuint
        }
        QQ2.NumberAnimation {
            target: clipPlane2
            property: "center.x"
            from: 0
            to: -10
            duration: 2000
            easing.type: QQ2.Easing.InOutQuint
        }
    }
}
