// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#version 450

layout(location = 0) in vec4 positionInLightSpace;
layout(location = 1) in vec3 position;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform qt3d_render_view_uniforms {
  mat4 viewMatrix;
  mat4 projectionMatrix;
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

layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelView;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 mvp;
  mat4 inverseModelViewProjectionMatrix;
  mat3 modelViewNormal;
};

layout(std140, binding = 2) uniform qt3d_custom_uniforms {
 mat4 lightViewProjection;
 vec3 lightPosition;
 vec3 lightIntensity;

 vec3 ka;            // Ambient reflectivity
 vec3 kd;            // Diffuse reflectivity
 vec3 ks;            // Specular reflectivity
 float shininess;    // Specular shininess factor
};

layout(binding = 3) uniform sampler2DShadow shadowMapTexture;


vec3 dsModel(const in vec3 pos, const in vec3 n)
{
    // Calculate the vector from the light to the fragment
    vec3 s = normalize(vec3(viewMatrix * vec4(lightPosition, 1.0)) - pos);

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize(-pos);

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect(-s, n);

    // Calculate the diffuse component
    float diffuse = max(dot(s, n), 0.0);

    // Calculate the specular component
    float specular = 0.0;
    if (dot(s, n) > 0.0)
        specular = pow(max(dot(r, v), 0.0), shininess);

    // Combine the diffuse and specular contributions (ambient is taken into account by the caller)
    return lightIntensity * (kd * diffuse + ks * specular);
}

void main()
{
    float shadowMapSample = textureProj(shadowMapTexture, positionInLightSpace);

    vec3 ambient = lightIntensity * ka;

    vec3 result = ambient;
    if (shadowMapSample > 0)
        result += dsModel(position, normalize(normal));

    fragColor = vec4(result, 1.0);
}
