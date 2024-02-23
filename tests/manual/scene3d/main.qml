// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0
import QtQuick.Scene3D 2.0
import Qt3D.Render 2.15

Item {
    Text {
        text: "Click me!"
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter

        MouseArea {
            anchors.fill: parent
            onClicked: animation.start()
        }
    }

    Text {
        text: "Multisample: " + scene3d.multisample
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter

        MouseArea {
            anchors.fill: parent
            onClicked: scene3d.multisample = !scene3d.multisample
        }
    }

    Rectangle {
        id: scene
        anchors.fill: parent
        anchors.margins: 50
        color: "darkRed"

        transform: Rotation {
            id: sceneRotation
            axis.x: 1
            axis.y: 0
            axis.z: 0
            origin.x: scene.width / 2
            origin.y: scene.height / 2
        }
        //! [0]
        Scene3D {
            id: scene3d
            anchors.fill: parent
            anchors.margins: 10
            focus: true
            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

            AnimatedEntity {
                id: rootEntity
            }
        }
        //! [0]
    }

    Rectangle {
        radius: 10
        color: "#aaffffff"
        border.width: 1
        border.color: "black"
        width: childrenRect.width + anchors.margins
        height: childrenRect.height + anchors.margins
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 20

        Column {
            x: parent.anchors.margins / 2
            y: x

            Text { text: "Vendor: " + rootEntity.capabilities.vendor }
            Text { text: "Renderer: " + rootEntity.capabilities.renderer }
            Text { text: "Driver Version: " + rootEntity.capabilities.driverVersion }
            Text { text: "GL Version: " + rootEntity.capabilities.majorVersion + "." + rootEntity.capabilities.minorVersion }
            Text { text: "Profile: " + (rootEntity.capabilities.profile === RenderCapabilities.CoreProfile ? "Core" : (rootEntity.capabilities.profile === RenderCapabilities.CompatibilityProfile ? "Compatibility" : "Unknown")) }
            Text { text: "GLSL Version: " + rootEntity.capabilities.glslVersion }
            Text { text: "Extensions: " + (rootEntity.capabilities.extensions.length ? "" : "None") }
            ListView {
                model: rootEntity.capabilities.extensions
                delegate: Text { text: "  " + model.modelData }
                width: parent.width
                height: 100
                visible: rootEntity.capabilities.extensions.length > 0
                clip: true
            }
            Text { text: "Max Texture Size: " + rootEntity.capabilities.maxTextureSize + "\nMax Texture Units: " + rootEntity.capabilities.maxTextureUnits + "\nMax Texture Layers: " + rootEntity.capabilities.maxTextureLayers }
            Text { text: "Supports UBO: " + rootEntity.capabilities.supportsUBO }
            Text { text: "  Max UBO Size: " + rootEntity.capabilities.maxUBOSize + "\n  Max UBO Bindings: " + rootEntity.capabilities.maxUBOBindings; visible: rootEntity.capabilities.supportsUBO }
            Text { text: "Supports SSBO: " + rootEntity.capabilities.supportsSSBO }
            Text { text: "  Max SSBO Size: " + rootEntity.capabilities.maxSSBOSize + "\n  Max SSBO Bindings: " + rootEntity.capabilities.maxSSBOBindings; visible: rootEntity.capabilities.supportsSSBO }
            Text { text: "Supports Image Store: " + rootEntity.capabilities.supportsImageStore }
            Text { text: "  Max Image Units: " + rootEntity.capabilities.maxImageUnits; visible: rootEntity.capabilities.supportsImageStore }
            Text { text: "Supports Compute Shaders: " + rootEntity.capabilities.supportsCompute }
            Text { text: "  Max Work Group Size: " + rootEntity.capabilities.maxWorkGroupSizeX + ", " + rootEntity.capabilities.maxWorkGroupSizeY + ", " + rootEntity.capabilities.maxWorkGroupSizeZ; visible: rootEntity.capabilities.supportsCompute }
            Text { text: "  Max Work Group Count: " + rootEntity.capabilities.maxWorkGroupCountX + ", " + rootEntity.capabilities.maxWorkGroupCountY + ", " + rootEntity.capabilities.maxWorkGroupCountZ; visible: rootEntity.capabilities.supportsCompute }
            Text { text: "  Max Invocations: " + rootEntity.capabilities.maxComputeInvocations; visible: rootEntity.capabilities.supportsCompute }
            Text { text: "  Max Shared Memory: " + rootEntity.capabilities.maxComputeSharedMemorySize; visible: rootEntity.capabilities.supportsCompute }
        }
    }

    Rectangle {
        radius: 10
        color: "#aaffffff"
        border.width: 1
        border.color: "black"
        width: childrenRect.width + anchors.margins
        height: childrenRect.height + anchors.margins
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 20
        visible: rootEntity.validBounds

        Column {
            x: parent.anchors.margins / 2
            y: x

            Text { text: "Sphere:\n  Min Extent: " + rootEntity.sphereMinPt }
            Text { text: "  Max Extent: " + rootEntity.sphereMaxPt }
        }
    }

    SequentialAnimation {
        id: animation

        RotationAnimation {
            to: 45
            duration: 1000
            target: sceneRotation
            property: "angle"
            easing.type: Easing.InOutQuad
        }
        PauseAnimation { duration: 500 }
        NumberAnimation {
            to: 0.5
            duration: 1000
            target: scene
            property: "scale"
            easing.type: Easing.OutElastic
        }
        PauseAnimation { duration: 500 }
        NumberAnimation {
            to: 1.0
            duration: 1000
            target: scene
            property: "scale"
            easing.type: Easing.OutElastic
        }
        PauseAnimation { duration: 500 }
        RotationAnimation {
            to: 0
            duration: 1000
            target: sceneRotation
            property: "angle"
            easing.type: Easing.InOutQuad
        }
    }
}
