// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

uniform highp mat4 viewMatrix;

uniform highp vec3 lightPosition;
uniform highp vec3 lightIntensity;

uniform highp vec3 ka;            // Ambient reflectivity
uniform highp vec3 ks;            // Specular reflectivity
uniform highp float shininess;    // Specular shininess factor
uniform highp float opacity;      // Alpha channel

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

varying highp vec4 positionInLightSpace;

varying highp vec3 lightDir;
varying highp vec3 viewDir;
varying highp vec2 texCoord;

highp vec3 dbModel(const highp vec3 norm, const highp vec2 flipYTexCoord)
{
    // Reflection of light direction about normal
    highp vec3 r = reflect(-lightDir, norm);

    highp vec3 diffuseColor = texture2D(diffuseTexture, flipYTexCoord).rgb;

    // Calculate the ambient contribution
    highp vec3 ambient = lightIntensity * ka * diffuseColor;

    // Calculate the diffuse contribution
    highp float sDotN = max(dot(lightDir, norm), 0.0);
    highp vec3 diffuse = lightIntensity * diffuseColor * sDotN;

    // Calculate the specular highlight contribution
    highp vec3 specular = vec3(0.0);
    if (sDotN > 0.0)
        specular = (lightIntensity * ks) * pow(max(dot(r, viewDir), 0.0), shininess);

    return ambient + diffuse + specular;
}

void main()
{
    highp vec2 flipYTexCoord = texCoord;
    flipYTexCoord.y = 1.0 - texCoord.y;

    highp vec4 normal = 2.0 * texture2D(normalTexture, flipYTexCoord) - vec4(1.0);

    highp vec3 result = dbModel(normalize(normal.xyz), flipYTexCoord);

    gl_FragColor = vec4(result, opacity);
}
