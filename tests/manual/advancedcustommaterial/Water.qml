// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.0 as QQ2


 Entity {
    id: water

    WaterMaterial {
        id: watermaterial

        property real tox: 0.0
        property real toy: 0.0
        property real vertY: 1.0
        property real waveRandomAnim: 0.0

        diffuse: "qrc:/textures/WaterDiffuse.jpg"
        normal: "qrc:/textures/WaterNormal.jpg"
        specular: "qrc:/textures/WaterSpecular.jpg"
        wave: "qrc:/textures/Waterwave.jpg"
        sky: "qrc:/textures/sky.jpg"
        foam: "qrc:/textures/foam.jpg"

        textureScale: slider1.value
        wavescale: vertY * slider2.value
        specularity: slider3.value
        offsetx: tox * slider5.value
        offsety: toy * slider5.value
        normalAmount: slider8.value
        waveheight: slider6.value
        waveStrenght: slider7.value
        shininess: 100
        waveRandom: waveRandomAnim
    }

    Mesh {
        id: watermesh
        source: "qrc:/models/waterPlane.obj"
    }

    Transform {
        id: waterTransform
        property real scale: 1.0
        property real rotx: 0.0
        scale3D: Qt.vector3d(scale, scale, scale)
        rotationY: slider4.value
    }

    Entity {
        id: waterEntity
        components: [watermesh, watermaterial, waterTransform]
    }

    QQ2.SequentialAnimation {
        QQ2.NumberAnimation {
            target: watermaterial
            property: "waveRandomAnim"
            to: 3.0
            duration: 4000
//            easing.type: Easing.Linear
        }
        QQ2.NumberAnimation {
            target: watermaterial
            property: "waveRandomAnim"
            to: 1.0
            duration: 4000
//            easing.type: Easing.Linear
        }
    }

    QQ2.SequentialAnimation {
        running: true
        loops: QQ2.Animation.Infinite
        QQ2.ParallelAnimation {
            QQ2.NumberAnimation {
                target: watermaterial
                property: "toy"
                to: 10.0
                duration: 100000
            }
            QQ2.NumberAnimation {
                target: watermaterial
                property: "tox"
                to: 10.0
                duration: 100000
            }
        }
        QQ2.ParallelAnimation {
            QQ2.NumberAnimation {
                target: watermaterial
                property: "toy"
                to: 0.0
                duration: 0
            }
            QQ2.NumberAnimation {
                target: watermaterial
                property: "tox"
                to: 0.0
                duration: 0
            }
        }
    }

    QQ2.SequentialAnimation {
        running: true
        loops: QQ2.Animation.Infinite
        QQ2.NumberAnimation {
            target: watermaterial
            property: "vertY"
            to: 200
            duration: 200000
//            easing.type: Easing.Linear
        }
        QQ2.NumberAnimation {
            target: watermaterial
            property: "vertY"
            to: 2
            duration: 200000
//            easing.type: Easing.Linear
        }
    }
}

