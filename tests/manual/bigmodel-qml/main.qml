// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.2 as QQ2

Entity {
    id: sceneRoot

    FirstPersonCameraController { camera: mainCamera }

    components: [
        RenderSettings {
            activeFrameGraph:ForwardRenderer {
                camera: mainCamera
            }
            pickingSettings {
                pickMethod: PickingSettings.TrianglePicking
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Camera {
        id: mainCamera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 22.5
        aspectRatio: 16 / 9
        nearPlane:   0.01
        farPlane:    1000.0
        position:   Qt.vector3d( 0.0, 25.0, 40.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
        upVector:   Qt.vector3d( 0.0, 1.0, 0.0 )
    }

    QQ2.ListModel {
        id: entityModel
        QQ2.ListElement { emptyRole: 0 }
    }

    NodeInstantiator {
        id: collection
        property int _count: 0
        property real spacing: 5
        property int cols: 8
        property int _rows: count / cols

        model: entityModel
        delegate: MyEntity {
            id: myEntity
            name: "E" + index
            property real _lightness: 0.2 + 0.7 / collection._rows * Math.floor(index / collection.cols)
            property real _hue: (index % collection.cols) / collection.cols
            position: Qt.vector3d(collection.spacing * (index % collection.cols - 0.5 * (collection.cols - 1)),
                                  0.0,
                                  collection.spacing * (Math.floor(index / collection.cols) - 0.5 * collection._rows));
            diffuse: Qt.hsla( _hue, 0.5, _lightness, 1.0 )
        }
    }

    QQ2.Timer {
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            entityModel.append({});
        }
    }
}
