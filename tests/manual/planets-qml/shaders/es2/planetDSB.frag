// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

uniform highp mat4 viewMatrix;

uniform highp vec3 lightPosition;
uniform highp vec3 lightIntensity;

uniform highp vec3 ka;            // Ambient reflectivity
uniform highp float shininess;    // Specular shininess factor
uniform highp float opacity;      // Alpha channel

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

varying highp vec4 positionInLightSpace;

varying highp vec3 lightDir;
varying highp vec3 viewDir;
varying highp vec2 texCoord;

highp vec3 dsbModel(const highp vec3 norm, const highp vec2 flipYTexCoord)
{
    // Reflection of light direction about normal
    highp vec3 r = reflect(-lightDir, norm);

    highp vec3 diffuseColor = texture2D(diffuseTexture, flipYTexCoord).rgb;
    highp vec3 specularColor = texture2D(specularTexture, flipYTexCoord).rgb;

    // Calculate the ambient contribution
    highp vec3 ambient = lightIntensity * ka * diffuseColor;

    // Calculate the diffuse contribution
    highp float sDotN = max(dot(lightDir, norm), 0.0);
    highp vec3 diffuse = lightIntensity * diffuseColor * sDotN;

    // Calculate the specular highlight contribution
    highp vec3 specular = vec3(0.0);
    if (sDotN > 0.0)
        specular = (lightIntensity * (shininess / (8.0 * 3.14))) * pow(max(dot(r, viewDir), 0.0), shininess);

    specular *= specularColor;

    return ambient + diffuse + specular;
}

void main()
{
    highp vec2 flipYTexCoord = texCoord;
    flipYTexCoord.y = 1.0 - texCoord.y;

    // Sample the textures at the interpolated texCoords
    highp vec4 normal = 2.0 * texture2D(normalTexture, flipYTexCoord) - vec4(1.0);

    highp vec3 result = dsbModel(normalize(normal.xyz), flipYTexCoord);

    // Combine spec with ambient+diffuse for final fragment color
    gl_FragColor = vec4(result, opacity);
}
