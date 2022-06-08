// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#version 450 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec2 vertexTexCoord;

layout(location = 3) out vec4 positionInLightSpace;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 texCoord;

layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelViewMatrix;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 modelViewProjection;
  mat4 inverseModelViewProjectionMatrix;
};

layout(std140, binding = 2) uniform extras_uniforms {
    float texCoordScale;
    vec3 ka;            // Ambient reflectivity
    vec3 ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
    float opacity;      // Alpha channel
    vec3 lightPosition;
    vec3 lightIntensity;
    mat4 lightViewProjection;
};

void main()
{
    const mat4 shadowMatrix = mat4(0.5, 0.0, 0.0, 0.0,
                                   0.0, 0.5, 0.0, 0.0,
                                   0.0, 0.0, 0.5, 0.0,
                                   0.5, 0.5, 0.5, 1.0);

    positionInLightSpace = shadowMatrix * lightViewProjection * modelMatrix * vec4(vertexPosition, 1.0);

    texCoord = vertexTexCoord * texCoordScale;
    normal = normalize(modelNormalMatrix * vertexNormal);
    position = vec3(modelViewMatrix * vec4(vertexPosition, 1.0));

    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
}
