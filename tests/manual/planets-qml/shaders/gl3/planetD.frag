// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#version 150 core

uniform mat4 viewMatrix;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;

uniform vec3 ka;            // Ambient reflectivity
uniform vec3 ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor
uniform float opacity;      // Alpha channel

uniform sampler2D diffuseTexture;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

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

    vec3 result = lightIntensity * ka * texture(diffuseTexture, flipYTexCoord).rgb;
    result += dModel(flipYTexCoord);

    float alpha = opacity * texture(diffuseTexture, flipYTexCoord).a;

    fragColor = vec4(result, alpha);
}
