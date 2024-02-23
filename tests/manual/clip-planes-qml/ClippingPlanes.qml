// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.4 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property Layer capsLayer
    property Layer visualizationLayer

    property ShaderData sectionData: ShaderData {
        property int sectionsCount: 3
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
        property Material cappingMaterial: Material {
            effect: CappingMaterialEffect {
                sectionsData: root.sectionData
            }
        }

        property GeometryRenderer mesh: PlaneMesh {
            width: 20.0
            height: 20.0
            meshResolution: Qt.size(2, 2)
        }

        components: [cappingMaterial, mesh, root.capsLayer]
    }

    PlaneVisualizationMaterial {
        id: clipPlanesMaterial
        alpha: 0.5
    }

    // XZ
    ClipPlaneEntity {
        id: clipPlane0
        layer: root.visualizationLayer
        visualMaterial: clipPlanesMaterial
        center: Qt.vector3d(0, -10, 0)
        normal: Qt.vector3d(0, -1.0, 0)
    }
    // XY
    ClipPlaneEntity {
        id: clipPlane1
        layer: root.visualizationLayer
        visualMaterial: clipPlanesMaterial
        center: Qt.vector3d(0, 0, 10)
        normal: Qt.vector3d(0, 0, -1.0)
        rotateAxis: Qt.vector3d(1.0, 0.0, 0.0)
        rotateAngle: 90
    }

    // YZ
    ClipPlaneEntity {
        id: clipPlane2
        layer: root.visualizationLayer
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
