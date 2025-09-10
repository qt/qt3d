// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma include light.inc.frag

void adsModel(const in FP vec3 vpos, const in FP vec3 vnormal, const in FP vec3 vview, const in FP float shininess,
              out FP vec3 ambientColor,
              out FP vec3 diffuseColor, out FP vec3 specularColor)
{
    ambientColor = vec3(0.0);
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);

    FP vec3 n = normalize( vnormal );

    FP vec3 s;
    Light light;
    for (int i = 0; i < lightCount; ++i) {
        if (i == 0)
            light = lights[0];
        else if (i == 1)
            light = lights[1];
        else if (i == 2)
            light = lights[2];
        else if (i == 3)
            light = lights[3];
        else if (i == 4)
            light = lights[4];
        else if (i == 5)
            light = lights[5];
        else if (i == 6)
            light = lights[6];
        else if (i == 7)
            light = lights[7];

        FP float att = 1.0;
        if ( light.type != TYPE_DIRECTIONAL ) {
            s = light.position - vpos;
            if (light.constantAttenuation != 0.0
              || light.linearAttenuation != 0.0
              || light.quadraticAttenuation != 0.0) {
                FP float dist = length(s);
                att = 1.0 / (light.constantAttenuation + light.linearAttenuation * dist + light.quadraticAttenuation * dist * dist);
            }
            s = normalize( s );
            if ( light.type == TYPE_SPOT ) {
                if ( degrees(acos(dot(-s, normalize(light.direction))) ) > light.cutOffAngle)
                    att = 0.0;
            }
        } else {
            s = normalize( -light.direction );
        }

        FP float diffuse = max( dot( s, n ), 0.0 );

        FP float specular = 0.0;
        if (diffuse > 0.0 && shininess > 0.0 && att > 0.0) {
            FP vec3 r = reflect( -s, n );
            FP float normFactor = ( shininess + 2.0 ) / 2.0;
            specular = normFactor * pow( max( dot( r, vview ), 0.0 ), shininess );
        }

        ambientColor += att * light.intensity * 1.0 * light.color;
        diffuseColor += att * light.intensity * diffuse * light.color;
        specularColor += att * light.intensity * specular * light.color;
    }
}

FP vec4 phongFunction(const in FP vec4 ambient,
                      const in FP vec4 diffuse,
                      const in FP vec4 specular,
                      const in FP float shininess,
                      const in FP vec3 worldPosition,
                      const in FP vec3 worldView,
                      const in FP vec3 worldNormal)
{
    // Calculate the lighting model, keeping the specular component separate
    FP vec3 ambientColor, diffuseColor, specularColor;
    adsModel(worldPosition, worldNormal, worldView, shininess, ambientColor, diffuseColor, specularColor);

    // Combine ambient, diffuse and specular components for final fragment color
    FP vec3 color = ambientColor * ambient.rgb
                  + diffuseColor * diffuse.rgb
                  + specularColor * specular.rgb;

    return vec4(color, diffuse.a);
}
