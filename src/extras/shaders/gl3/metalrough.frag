/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#version 150

in vec2 texCoord;
in vec3 worldPosition;
in vec3 worldNormal;
in vec4 worldTangent;

out vec4 fragColor;

// Qt 3D built in uniforms
uniform vec3 eyePosition; // World space eye position
uniform float time; // Time in seconds

// Pre-convolved environment maps
uniform samplerCube skyIrradiance;  // For diffuse contribution
uniform samplerCube skySpecular;    // For specular contribution

// PBR Material maps
uniform sampler2D baseColorMap;
uniform sampler2D metalnessMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D ambientOcclusionMap;

// User control parameters
uniform float metalFactor = 1.0;

// Roughness -> mip level mapping
uniform float maxT = 0.939824;
uniform float mipLevels = 11.0;
uniform float mipOffset = 5.0;

// Exposure correction
uniform float exposure = 0.0;
// Gamma correction
uniform float gamma = 2.2;

mat3 calcWorldSpaceToTangentSpaceMatrix(const in vec3 wNormal, const in vec4 wTangent)
{
    // Make the tangent truly orthogonal to the normal by using Gram-Schmidt.
    // This allows to build the tangentMatrix below by simply transposing the
    // tangent -> eyespace matrix (which would now be orthogonal)
    vec3 wFixedTangent = normalize(wTangent.xyz - dot(wTangent.xyz, wNormal) * wNormal);

    // Calculate binormal vector. No "real" need to renormalize it,
    // as built by crossing two normal vectors.
    // To orient the binormal correctly, use the fourth coordinate of the tangent,
    // which is +1 for a right hand system, and -1 for a left hand system.
    vec3 wBinormal = cross(wNormal, wFixedTangent.xyz) * wTangent.w;

    // Construct matrix to transform from world space to tangent space
    // This is the transpose of the tangentToWorld transformation matrix
    mat3 tangentToWorldMatrix = mat3(wFixedTangent, wBinormal, wNormal);
    mat3 worldToTangentMatrix = transpose(tangentToWorldMatrix);
    return worldToTangentMatrix;
}

float roughnessToMipLevel(float roughness)
{
    // HACK: Improve the roughness -> mip level mapping for roughness map from substace painter
    // TODO: Use mathematica or similar to improve this mapping more generally
    roughness = 0.75 + (1.7 * (roughness - 0.5));
    return (mipLevels - 1.0 - mipOffset) * (1.0 - (1.0 - roughness) / maxT);
}

vec3 fresnelFactor(const in vec3 color, const in float cosineFactor)
{
    // Calculate the Fresnel effect value
    vec3 f = color;
    vec3 F = f + (1.0 - f) * pow(1.0 - cosineFactor, 5.0);
    return clamp(F, f, vec3(1.0));
}

float geometricModel(const in float lDotN,
                     const in float vDotN,
                     const in vec3 h)
{
    // Implicit geometric model (equal to denominator in specular model).
    // This currently assumes that there is no attenuation by geometric shadowing or
    // masking according to the microfacet theory.
    return 1.0;
}

vec3 specularModel(const in vec3 F0,
                   const in float lDotH,
                   const in float lDotN,
                   const in float vDotN,
                   const in vec3 n,
                   const in vec3 h)
{
    // Clamp lDotN and vDotN to small positive value to prevent the
    // denominator in the reflection equation going to infinity. Balance this
    // by using the clamped values in the geometric factor function to
    // avoid ugly seams in the specular lighting.
    float sDotNPrime = max(lDotN, 0.001);
    float vDotNPrime = max(vDotN, 0.001);

    vec3 F = fresnelFactor(F0, lDotH);
    float G = geometricModel(sDotNPrime, vDotNPrime, h);

    // TODO: Verify which parts of the BRDF Lys is preconvolving and multiply
    // by the remaining factors here.
    vec3 cSpec = F * G / (4.0 * sDotNPrime * vDotNPrime);
    return clamp(cSpec, vec3(0.0), vec3(1.0));
}

vec3 pbrIblModel(const in vec3 wNormal,
                 const in vec3 wView,
                 const in vec3 baseColor,
                 const in float metalness,
                 const in float roughness,
                 const in float ambientOcclusion)
{
    // Calculate reflection direction of view vector about surface normal
    // vector in world space. This is used in the fragment shader to sample
    // from the environment textures for a light source. This is equivalent
    // to the l vector for punctual light sources. Armed with this, calculate
    // the usual factors needed
    vec3 n = wNormal;
    vec3 l = reflect(-wView, n);
    vec3 v = wView;
    vec3 h = normalize(l + v);
    float vDotN = dot(v, n);
    float lDotN = dot(l, n);
    float lDotH = dot(l, h);

    // Calculate diffuse component
    vec3 diffuseColor = (1.0 - metalness) * baseColor;
    vec3 diffuse = diffuseColor * texture(skyIrradiance, l).rgb;

    // Calculate specular component
    vec3 dielectricColor = vec3(0.04);
    vec3 F0 = mix(dielectricColor, baseColor, metalness);
    vec3 specularFactor = specularModel(F0, lDotH, lDotN, vDotN, n, h);

    float lod = roughnessToMipLevel(roughness);
    vec3 specularSkyColor = textureLod(skySpecular, l, lod).rgb;
    vec3 specular = specularSkyColor * specularFactor;

    // Blend between diffuse and specular to conserve energy
    vec3 iblColor = specular + diffuse * (vec3(1.0) - specularFactor);

    // Reduce by ambient occlusion amount
    iblColor *= ambientOcclusion;

    // Apply exposure correction
    iblColor *= pow(2.0, exposure);

    return iblColor;
}

vec3 toneMap(const in vec3 c)
{
    return c / (c + vec3(1.0));
}

vec3 gammaCorrect(const in vec3 color)
{
    return pow(color, vec3(1.0 / gamma));
}

void main()
{
    // Calculate the perturbed texture coordinates from parallax occlusion mapping
    mat3 worldToTangentMatrix = calcWorldSpaceToTangentSpaceMatrix(worldNormal, worldTangent);
    vec3 wView = normalize(eyePosition - worldPosition);
    vec3 tView = worldToTangentMatrix * wView;

    // Sample the inputs needed for the metal-roughness PBR BRDF
    vec3 baseColor = texture(baseColorMap, texCoord).rgb;
    float metalness = texture(metalnessMap, texCoord).r * metalFactor;
    float roughness = texture(roughnessMap, texCoord).r;
    float ambientOcclusion = texture(ambientOcclusionMap, texCoord).r;
    vec3 tNormal = 2.0 * texture(normalMap, texCoord).rgb - vec3(1.0);
    vec3 wNormal = normalize(transpose(worldToTangentMatrix) * tNormal);

    vec3 cLinear = pbrIblModel(wNormal,
                               wView,
                               baseColor,
                               metalness,
                               roughness,
                               ambientOcclusion);

    // Apply simple (Reinhard) tonemap transform to get into LDR range [0, 1]
    vec3 cToneMapped = toneMap(cLinear);

    // Apply gamma correction prior to display
    vec3 cGamma = gammaCorrect(cToneMapped);
    fragColor = vec4(cGamma, 1.0);
}
