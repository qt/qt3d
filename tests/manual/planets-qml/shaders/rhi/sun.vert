// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#version 450 core


layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec2 vertexTexCoord;

layout(location=0) out vec3 position;
layout(location=1) out vec2 texCoord;

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
};

void main()
{
    texCoord = vertexTexCoord * texCoordScale;
    position = vec3(modelViewMatrix * vec4(vertexPosition, 1.0));
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
}
