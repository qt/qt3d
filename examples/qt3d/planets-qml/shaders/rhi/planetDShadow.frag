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
layout(binding=4) uniform sampler2DShadow shadowMapTexture;

layout(location = 3) in vec4 positionInLightSpace;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(location = 0)out vec4 fragColor;

vec3 dModel(const in vec2 flipYTexCoord)
{
    // Calculate the vector from the light to the fragment
    vec3 s = normalize(vec3(viewMatrix * vec4(lightPosition, 1.0)) - position);

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize(-position);

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect(-s, normal);

    // Calculate the diffuse component
    float diffuse = max(dot(s, normal), 0.0);

    // Calculate the specular component
    float specular = 0.0;
    if (dot(s, normal) > 0.0)
        specular = (shininess / (8.0 * 3.14)) * pow(max(dot(r, v), 0.0), shininess);

    // Lookup diffuse and specular factors
    vec3 diffuseColor = texture(diffuseTexture, flipYTexCoord).rgb;

    // Combine the ambient, diffuse and specular contributions
    return lightIntensity * ((ka + diffuse) * diffuseColor + specular * ks);
}

void main()
{
    vec2 flipYTexCoord = texCoord;
    flipYTexCoord.y = 1.0 - texCoord.y;

    float shadowMapSample = textureProj(shadowMapTexture, positionInLightSpace);

    vec3 result = lightIntensity * ka * texture(diffuseTexture, flipYTexCoord).rgb;
    if (shadowMapSample > 0)
        result += dModel(flipYTexCoord);

    float alpha = opacity * texture(diffuseTexture, flipYTexCoord).a;

    fragColor = vec4(result, alpha);
}
