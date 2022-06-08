// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    readonly property Layer layer: screenQuadLayer
    property PointLight baseLight
    property vector3d light1Pos
    property vector3d light2Pos
    property vector3d light3Pos
    property vector3d light4Pos

    Entity {
        components: [
            baseLight,
            layer
        ]
    }

    Entity {
        readonly property Transform transform1 : Transform { translation: root.light1Pos }
        readonly property PointLight light1 :  PointLight {
            color : "dodgerblue"
            intensity : 0.9
        }
        components: [
            transform1,
            light1,
            layer
        ]
    }

    Entity {
        readonly property Transform transform2 : Transform { translation: root.light2Pos }
        readonly property PointLight light2 :  PointLight {
            color : "green"
            intensity : 0.5
        }
        components: [
            transform2,
            light2,
            layer
        ]
    }

    Entity {
        readonly property Transform transform3 : Transform { translation: root.light3Pos }
        readonly property PointLight light3 :  PointLight {
            color : "white"
            intensity : 2.0
        }
        components: [
            transform3,
            light3,
            layer
        ]
    }

    Entity {
        readonly property Transform transform4 : Transform { translation: root.light4Pos }
        readonly property PointLight light4 :  PointLight {
            color : "white"
            intensity : 0.5
        }
        components: [
            transform4,
            light4,
            layer
        ]
    }

    // We need to have the actual screen quad entity separate from the lights.
    // If the lights were sub-entities of this screen quad entity, they would
    // be affected by the rotation matrix, and their world positions would thus
    // be changed.
    Entity {
        components : [
            Layer { id: screenQuadLayer },

            PlaneMesh {
                width: 2.0
                height: 2.0
                meshResolution: Qt.size(2, 2)
            },

            Transform { // We rotate the plane so that it faces us
                rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 90)
            },

            Material {
                effect : FinalEffect {}
            }
        ]
    }
}
