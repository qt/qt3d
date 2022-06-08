// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#define FP highp

FP mat3 transpose(const in FP mat3 inputMatrix)
{
    FP vec3 i0 = inputMatrix[0];
    FP vec3 i1 = inputMatrix[1];
    FP vec3 i2 = inputMatrix[2];

    FP mat3 outputMatrix = mat3(
        vec3(i0.x, i1.x, i2.x),
        vec3(i0.y, i1.y, i2.y),
        vec3(i0.z, i1.z, i2.z)
    );

    return outputMatrix;
}

FP mat3 calcWorldSpaceToTangentSpaceMatrix(const in FP vec3 wNormal, const in FP vec4 wTangent)
{
    // Make the tangent truly orthogonal to the normal by using Gram-Schmidt.
    // This allows to build the tangentMatrix below by simply transposing the
    // tangent -> eyespace matrix (which would now be orthogonal)
    FP vec3 wFixedTangent = normalize(wTangent.xyz - dot(wTangent.xyz, wNormal) * wNormal);

    // Calculate binormal vector. No "real" need to renormalize it,
    // as built by crossing two normal vectors.
    // To orient the binormal correctly, use the fourth coordinate of the tangent,
    // which is +1 for a right hand system, and -1 for a left hand system.
    FP vec3 wBinormal = cross(wNormal, wFixedTangent.xyz) * wTangent.w;

    // Construct matrix to transform from world space to tangent space
    // This is the transpose of the tangentToWorld transformation matrix
    FP mat3 tangentToWorldMatrix = mat3(wFixedTangent, wBinormal, wNormal);
    FP mat3 worldToTangentMatrix = transpose(tangentToWorldMatrix);
    return worldToTangentMatrix;
}

