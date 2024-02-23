// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

uniform sampler2D diffuseTexture;

varying highp vec3 position;
varying highp vec2 texCoord;

highp vec4 dModel(const highp vec2 flipYTexCoord)
{
    // Lookup diffuse
    highp vec3 diffuseColor = texture2D(diffuseTexture, flipYTexCoord).rgb;

    return vec4(diffuseColor, 1.0);
}

void main()
{
    highp vec2 flipYTexCoord = texCoord;
    flipYTexCoord.y = 1.0 - texCoord.y;

    highp vec4 result = dModel(flipYTexCoord);

    gl_FragColor = result;
}
