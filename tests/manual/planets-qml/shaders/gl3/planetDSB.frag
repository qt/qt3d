// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#version 150 core

uniform mat4 viewMatrix;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;

uniform vec3 ka;            // Ambient reflectivity
uniform float shininess;    // Specular shininess factor
uniform float opacity;      // Alpha channel

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord;

out vec4 fragColor;

void dsbModel(const in vec3 norm, const in vec2 flipXTexCoord, out vec3 ambientAndDiff, out vec3 spec)
{
    // Reflection of light direction about normal
    vec3 r = reflect(-lightDir, norm);

    vec3 diffuseColor = texture(diffuseTexture, flipXTexCoord).rgb;
    vec3 specularColor = texture(specularTexture, flipXTexCoord).rgb;

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
        spec = (lightIntensity * (shininess / (8.0 * 3.14))) * pow(max(dot(r, viewDir), 0.0), shininess);

    spec *= specularColor;
}

void main()
{
    vec2 flipXTexCoord = texCoord;
    flipXTexCoord.x = 1.0 - texCoord.x;

    // Sample the textures at the interpolated texCoords
    vec4 normal = 2.0 * texture(normalTexture, flipXTexCoord) - vec4(1.0);

    // Calculate the lighting model, keeping the specular component separate
    vec3 ambientAndDiff, spec;
    dsbModel(normalize(normal.xyz), flipXTexCoord, ambientAndDiff, spec);
    vec3 result = ambientAndDiff + spec;

    // Combine spec with ambient+diffuse for final fragment color
    fragColor = vec4(result, opacity);
}
