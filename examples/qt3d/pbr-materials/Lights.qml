// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.1
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: lights

    // Point Light (pulsing)
    Entity {
        components: [
            PointLight {
                color: "red"
                intensity: 0.3
                constantAttenuation: 1.0
                linearAttenuation: 0.0
                quadraticAttenuation: 0.0025

                NumberAnimation on intensity {
                    from: 0.3; to: 0.8;
                    running: true
                    loops: Animation.Infinite
                    duration: 1000
                    easing.type: Easing.CosineCurve
                }
            },
            Transform {
                translation: Qt.vector3d(0.0, 5.0, 0.0)
            }
        ]
    }

    // 2 x Directional Lights (steady)
    Entity {
        components: [
            DirectionalLight {
                worldDirection: Qt.vector3d(0.3, -3.0, 0.0).normalized();
                color: "#fbf9ce"
                intensity: 0.3
            }
        ]
    }

    Entity {
        components: [
            DirectionalLight {
                worldDirection: Qt.vector3d(-0.3, -0.3, 0.0).normalized();
                color: "#9cdaef"
                intensity: 0.15
            }
        ]
    }

    // Spot Light (sweeping)
    Entity {
        components: [
            SpotLight {
                localDirection: Qt.vector3d(0.0, -1.0, 0.0)
                color: "white"
                intensity: 5.0
            },
            Transform {
                id: spotLightTransform
                translation: Qt.vector3d(0.0, 5.0, 0.0)
                rotationZ: -60.0


                SequentialAnimation {
                    loops: Animation.Infinite
                    running: true

                    NumberAnimation {
                        target: spotLightTransform
                        property: "rotationZ"
                        from: -60; to: 60
                        duration: 4000
                        easing.type: Easing.InOutQuad
                    }

                    NumberAnimation {
                        target: spotLightTransform
                        property: "rotationZ"
                        from: 60; to: -60
                        duration: 4000
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        ]
    }
}
