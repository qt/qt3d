// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#version 150

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexTangent;
in vec2 vertexTexCoord;
in ivec4 vertexJointIndices;
in vec4 vertexJointWeights;

out vec3 worldPosition;
out vec3 worldNormal;
out vec4 worldTangent;
out vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 mvp;

const int maxJoints = 100;
uniform mat4 skinningPalette[maxJoints];

void main()
{
    // Pass the texture coordinates through
    texCoord = vertexTexCoord;

    // Perform the skinning
    mat4 skinningMatrix = skinningPalette[vertexJointIndices[0]] * vertexJointWeights[0];
    skinningMatrix     += skinningPalette[vertexJointIndices[1]] * vertexJointWeights[1];
    skinningMatrix     += skinningPalette[vertexJointIndices[2]] * vertexJointWeights[2];
    skinningMatrix     += skinningPalette[vertexJointIndices[3]] * vertexJointWeights[3];

    vec4 skinnedPosition = skinningMatrix * vec4(vertexPosition, 1.0);
    vec3 skinnedNormal = vec3(skinningMatrix * vec4(vertexNormal, 0.0));
    vec3 skinnedTangent = vec3(skinningMatrix * vec4(vertexTangent.xyz, 0.0));

    // Transform position, normal, and tangent to world space
    worldPosition = vec3(modelMatrix * skinnedPosition);
    worldNormal = normalize(modelNormalMatrix * skinnedNormal);
    worldTangent.xyz = normalize(vec3(modelMatrix * vec4(skinnedTangent, 0.0)));
    worldTangent.w = vertexTangent.w;

    gl_Position = mvp * skinnedPosition;
}
