// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#version 450 core

layout(std140, binding = 0) uniform qt3d_render_view_uniforms {
  mat4 viewMatrix;
  mat4 projectionMatrix;
  mat4 uncorrectedProjectionMatrix;
  mat4 clipCorrectionMatrix;
  mat4 viewProjectionMatrix;
  mat4 inverseViewMatrix;
  mat4 inverseProjectionMatrix;
  mat4 inverseViewProjectionMatrix;
  mat4 viewportMatrix;
  mat4 inverseViewportMatrix;
  vec4 textureTransformMatrix;
  vec3 eyePosition;
  float aspectRatio;
  float gamma;
  float exposure;
  float time;
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

layout(binding=3) uniform sampler2D diffuseTexture;
layout(binding=4) uniform sampler2D normalTexture;

layout(location = 0) in vec3 lightDir;
layout(location = 1) in vec3 viewDir;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

void dbModel(const in vec3 norm, const in vec2 flipYTexCoord, out vec3 ambientAndDiff, out vec3 spec)
{
    // Reflection of light direction about normal
    vec3 r = reflect(-lightDir, norm);

    vec3 diffuseColor = texture(diffuseTexture, flipYTexCoord).rgb;

    // Calculate the ambient contribution
    vec3 ambient = lightIntensity * ka * diffuseColor;

    // Calculate the diffuse contribution
    float sDotN = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = lightIntensity * diffuseColor * sDotN;

    // Sum the ambient and diffuse contributions
    ambientAndDiff = ambient + diffuse;

    // Calculate the specular highlight contribution
    spec = vec3(0.0);
    if (sDotN > 0.0)
        spec = (lightIntensity * ks) * pow(max(dot(r, viewDir), 0.0), shininess);
}

void main()
{
    vec2 flipYTexCoord = texCoord;
    flipYTexCoord.y = 1.0 - texCoord.y;

    // Sample the textures at the interpolated texCoords
    vec4 normal = 2.0 * texture(normalTexture, flipYTexCoord) - vec4(1.0);

    vec3 result = lightIntensity * ka * texture(diffuseTexture, flipYTexCoord).rgb;

    // Calculate the lighting model, keeping the specular component separate
    vec3 ambientAndDiff, spec;
    dbModel(normalize(normal.xyz), flipYTexCoord, ambientAndDiff, spec);
    result = ambientAndDiff + spec;

    // Combine spec with ambient+diffuse for final fragment color
    fragColor = vec4(result, opacity);
}
