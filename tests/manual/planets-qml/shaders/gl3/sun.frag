// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#version 150 core

uniform sampler2D diffuseTexture;

in vec3 position;
in vec2 texCoord;

out vec4 fragColor;

vec4 dModel(const in vec2 flipYTexCoord)
{
    // Lookup diffuse
    vec3 diffuseColor = texture(diffuseTexture, flipYTexCoord).rgb;

    return vec4(diffuseColor, 1.0);
}

void main()
{
    vec2 flipYTexCoord = texCoord;
    flipYTexCoord.y = 1.0 - texCoord.y;

    vec4 result = dModel(flipYTexCoord);

    fragColor = result;
}
