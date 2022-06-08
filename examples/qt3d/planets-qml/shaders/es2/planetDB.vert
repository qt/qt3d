// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexCoord;
attribute vec4 vertexTangent;

varying vec4 positionInLightSpace;
varying vec3 lightDir;
varying vec3 viewDir;
varying vec2 texCoord;

uniform mat4 viewMatrix;
uniform mat4 lightViewProjection;
uniform mat4 modelMatrix;
uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

uniform float texCoordScale;

uniform vec3 lightPosition;

void main()
{
    const mat4 shadowMatrix = mat4(0.5, 0.0, 0.0, 0.0,
                                   0.0, 0.5, 0.0, 0.0,
                                   0.0, 0.0, 0.5, 0.0,
                                   0.5, 0.5, 0.5, 1.0);

    positionInLightSpace = shadowMatrix * lightViewProjection * modelMatrix * vec4(vertexPosition, 1.0);

    // Pass through texture coordinates
    texCoord = vertexTexCoord * texCoordScale;

    // Transform position, normal, and tangent to eye coords
    vec3 normal = normalize(modelViewNormal * vertexNormal);
    vec3 tangent = normalize(modelViewNormal * vertexTangent.xyz);
    vec3 position = vec3(modelView * vec4(vertexPosition, 1.0));

    // Calculate binormal vector
    vec3 binormal = normalize(cross(normal, tangent));

    // Construct matrix to transform from eye coords to tangent space
    mat3 tangentMatrix = mat3 (
        tangent.x, binormal.x, normal.x,
        tangent.y, binormal.y, normal.y,
        tangent.z, binormal.z, normal.z);

    // Transform light direction and view direction to tangent space
    vec3 s = lightPosition - position;
    lightDir = normalize(tangentMatrix * vec3(viewMatrix * vec4(s, 1.0)));

    vec3 v = -position;
    viewDir = normalize(tangentMatrix * v);

    // Calculate vertex position in clip coordinates
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
