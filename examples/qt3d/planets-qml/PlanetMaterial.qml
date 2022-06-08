// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.1

Material {
    id: root
    property color ambientLight: Qt.rgba(0.1, 0.1, 0.1, 1.0)
    property color diffuseColor: Qt.rgba(0.7, 0.7, 0.7, 1.0)
    property color specularColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)
    property real shininess: 150.0
    property string diffuseMap
    property string specularMap
    property string normalMap
    property real textureScale: 1.0
    property real opacity: 1.0

    parameters: [
        Parameter {
            name: "ambient"
            value: Qt.vector3d(root.ambientLight.r, root.ambientLight.g, root.ambientLight.b)
        },
        Parameter {
            name: "diffuse"
            value: Qt.vector3d(root.diffuseColor.r, root.diffuseColor.g, root.diffuseColor.b)
        },
        Parameter {
            name: "specular"
            value: Qt.vector3d(root.specularColor.r, root.specularColor.g, root.specularColor.b)
        },
        Parameter {
            name: "shininess"
            value: root.shininess
        },
        Parameter {
            name: "diffuseTexture"
            value: Texture2D {
                id: diffuseTexture
                minificationFilter: Texture.LinearMipMapLinear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.Repeat
                    y: WrapMode.Repeat
                }
                generateMipMaps: true
                maximumAnisotropy: 16.0
                TextureImage {
                    id: diffuseTextureImage
                    source: diffuseMap
                }
            }
        },
        Parameter {
            name: "specularTexture"
            value: Texture2D {
                id: specularTexture
                minificationFilter: Texture.LinearMipMapLinear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.Repeat
                    y: WrapMode.Repeat
                }
                generateMipMaps: true
                maximumAnisotropy: 16.0
                TextureImage {
                    id: specularTextureImage
                    // Get rid of runtime warnings. It's safe, as the texture just isn't used
                    source: specularMap !== "" ? specularMap
                                               : "qrc:/images/solarsystemscope/uranusmap.jpg"
                }
            }
        },
        Parameter {
            name: "normalTexture"
            value: Texture2D {
                id: normalTexture
                minificationFilter: Texture.Linear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.Repeat
                    y: WrapMode.Repeat
                }
                maximumAnisotropy: 16.0
                TextureImage {
                    id: normalTextureImage
                    // Get rid of runtime warnings. It's safe, as the texture just isn't used
                    source: normalMap !== "" ? normalMap
                                             : "qrc:/images/solarsystemscope/uranusmap.jpg"
                }
            }
        },
        Parameter {
            name: "texCoordScale"
            value: textureScale
        },
        Parameter {
            name: "opacity"
            value: opacity
        }
    ]
}
