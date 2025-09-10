// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef FP
#define FP highp
#endif

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
