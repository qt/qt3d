// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0
import QtQuick.Scene3D 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1

Item {

    Scene3D {
        anchors.fill: parent
        aspects: "input"
        ParticlesScene {
            id: scene
            particleStep: stepSlider.value
            finalCollisionFactor: collisionSlider.value
        }
    }

    ColumnLayout {
        id: colorLayout
        anchors.left: parent.left
        anchors.leftMargin: 35
        anchors.right: parent.right
        anchors.rightMargin: 35
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 35
        spacing: 15

        RowLayout {
            Text {
                text: "Particles Step:"
                color: "white"
            }
            Slider {
                height: 35
                id: stepSlider
                Layout.fillWidth: true
                from: 0.0
                to: 2
                value: 0.4
            }
        }
        RowLayout {
            Text {
                text: "Particles Collision:"
                color: "white"
            }
            Slider {
                height: 35
                id: collisionSlider
                Layout.fillWidth: true
                from: 0.0
                to: 2
                value: 0.2
            }
        }
        RowLayout {
            Button {
                text: "Reset Particles"
               onClicked: scene.reset()
            }
        }
        RowLayout {
            Text {
                text: "Particles Shape:"
                color: "white"
            }
            ButtonGroup {
                exclusive: true
                id: particlesTypeGroup
            }
            CheckBox {
                text: "Sphere"
                checked: true
                ButtonGroup.group: particlesTypeGroup
                onClicked: scene.particlesShape = scene._SPHERE
            }
            CheckBox
            { text: "Cube"
                checked: false
                ButtonGroup.group: particlesTypeGroup
                onClicked: scene.particlesShape = scene._CUBE
            }
            CheckBox {
                text: "Cylinder"
                checked: false
                ButtonGroup.group: particlesTypeGroup
                onClicked: scene.particlesShape = scene._CYLINDER
            }
            CheckBox {
                text: "Torus"
                checked: false
                ButtonGroup.group: particlesTypeGroup
                onClicked: scene.particlesShape = scene._TORUS
            }
        }
    }
}
