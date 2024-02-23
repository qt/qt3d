// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#version 450 core

layout(binding=3) uniform sampler2D diffuseTexture;

layout(location=0) in vec3 position;
layout(location=1) in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

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
