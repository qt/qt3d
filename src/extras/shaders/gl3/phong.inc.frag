// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma include light.inc.frag

void adsModel(const in vec3 worldPos,
              const in vec3 worldNormal,
              const in vec3 worldView,
              const in float shininess,
              out vec3 ambientColor,
              out vec3 diffuseColor,
              out vec3 specularColor)
{
    ambientColor = vec3(0.0);
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);

    // We perform all work in world space
    vec3 n = normalize(worldNormal);
    vec3 s = vec3(0.0);

    for (int i = 0; i < lightCount; ++i) {
        float att = 1.0;
        float sDotN = 0.0;

        if (lights[i].type != TYPE_DIRECTIONAL) {
            // Point and Spot lights

            // Light position is already in world space
            vec3 sUnnormalized = lights[i].position - worldPos;
            s = normalize(sUnnormalized); // Light direction

            // Calculate the attenuation factor
            sDotN = dot(s, n);
            if (sDotN > 0.0) {
                if (lights[i].constantAttenuation != 0.0
                 || lights[i].linearAttenuation != 0.0
                 || lights[i].quadraticAttenuation != 0.0) {
                    float dist = length(sUnnormalized);
                    att = 1.0 / (lights[i].constantAttenuation +
                                 lights[i].linearAttenuation * dist +
                                 lights[i].quadraticAttenuation * dist * dist);
                }

                // The light direction is in world space already
                if (lights[i].type == TYPE_SPOT) {
                    // Check if fragment is inside or outside of the spot light cone
                    if (degrees(acos(dot(-s, lights[i].direction))) > lights[i].cutOffAngle)
                        sDotN = 0.0;
                }
            }
        } else {
            // Directional lights
            // The light direction is in world space already
            s = normalize(-lights[i].direction);
            sDotN = dot(s, n);
        }

        // Calculate the diffuse factor
        float diffuse = max(sDotN, 0.0);

        // Calculate the specular factor
        float specular = 0.0;
        if (diffuse > 0.0 && shininess > 0.0) {
            float normFactor = (shininess + 2.0) / 2.0;
            vec3 r = reflect(-s, n);   // Reflection direction in world space
            specular = normFactor * pow(max(dot(r, worldView), 0.0), shininess);
        }

        // Accumulate the ambient, diffuse and specular contributions
        ambientColor += att * lights[i].intensity * 1.0 * lights[i].color;
        diffuseColor += att * lights[i].intensity * diffuse * lights[i].color;
        specularColor += att * lights[i].intensity * specular * lights[i].color;
    }
}

vec4 phongFunction(const in vec4 ambient,
                   const in vec4 diffuse,
                   const in vec4 specular,
                   const in float shininess,
                   const in vec3 worldPosition,
                   const in vec3 worldView,
                   const in vec3 worldNormal)
{
    // Calculate the lighting model, keeping the specular component separate
    vec3 ambientColor, diffuseColor, specularColor;
    adsModel(worldPosition, worldNormal, worldView, shininess, ambientColor, diffuseColor, specularColor);

    // Combine ambient, diffuse and specular components for final fragment color
    vec3 color = ambientColor * ambient.rgb
               + diffuseColor * diffuse.rgb
               + specularColor * specular.rgb;

    return vec4(color, diffuse.a);
}
