const int MAX_LIGHTS = 8;
struct Light {
    FP vec3 position;
    FP vec3 color;
    FP float intensity;
    FP vec3 direction;
    FP vec3 attenuation;
//    FP float cutOffAngle;
};
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

void adsModelNormalMapped(const in FP vec3 vpos, const in FP vec3 vnormal, const in FP vec3 eye, const in FP float shininess,
                          const in FP mat3 tangentMatrix,
                          out FP vec3 diffuseColor, out FP vec3 specularColor)
{
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);

    FP vec3 n = normalize( vnormal );

    // TODO dynamic indexing may not be supported with GLSL 1.00 so take only the first light into account
    FP vec3 lightDir = lights[0].direction;
    FP float att = 1.0;
    if (length( lightDir ) == 0.0) {
        lightDir = lights[0].position - vpos;
        if (length( lights[0].attenuation ) != 0.0) {
            FP float dist = length(lightDir);
            att = 1.0 / (lights[0].attenuation.x + lights[0].attenuation.y * dist + lights[0].attenuation.z * dist * dist);
        }
    }

    FP vec3 s = normalize( tangentMatrix * lightDir );
    FP float diffuse = max( dot( s, n ), 0.0 );

    FP float specular = 0.0;
    if (diffuse > 0.0 && shininess > 0.0) {
        FP vec3 r = reflect( -s, n );
        FP vec3 v = normalize( tangentMatrix * ( eye - vpos ) );
        FP float normFactor = ( shininess + 2.0 ) / 2.0;
        specular = normFactor * pow( max( dot( r, v ), 0.0 ), shininess );
    }

    diffuseColor += att * lights[0].intensity * diffuse * lights[0].color;
    specularColor += specular;
}

void adsModel(const in FP vec3 vpos, const in FP vec3 vnormal, const in FP vec3 eye, const in FP float shininess,
              out FP vec3 diffuseColor, out FP vec3 specularColor)
{
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);

    FP vec3 n = normalize( vnormal );

    // TODO dynamic indexing may not be supported with GLSL 1.00 so take only the first light into account
    FP vec3 lightDir = lights[0].direction;
    FP float att = 1.0;
    if (length( lightDir ) == 0.0) {
        lightDir = lights[0].position - vpos;
        if (length( lights[0].attenuation ) != 0.0) {
            FP float dist = length(lightDir);
            att = 1.0 / (lights[0].attenuation.x + lights[0].attenuation.y * dist + lights[0].attenuation.z * dist * dist);
        }
    }

    FP vec3 s = normalize( lightDir );
    FP float diffuse = max( dot( s, n ), 0.0 );

    FP float specular = 0.0;
    if (diffuse > 0.0 && shininess > 0.0) {
        FP vec3 r = reflect( -s, n );
        FP vec3 v = normalize( eye - vpos );
        FP float normFactor = ( shininess + 2.0 ) / 2.0;
        specular = normFactor * pow( max( dot( r, v ), 0.0 ), shininess );
    }

    diffuseColor += att * lights[0].intensity * diffuse * lights[0].color;
    specularColor += specular;
}

void adModel(const in FP vec3 vpos, const in FP vec3 vnormal, out FP vec3 diffuseColor)
{
    diffuseColor = vec3(0.0);

    FP vec3 n = normalize( vnormal );

    // TODO dynamic indexing may not be supported with GLSL 1.00 so take only the first light into account
    FP vec3 lightDir = lights[0].direction;
    FP float att = 1.0;
    if (length( lightDir ) == 0.0) {
        lightDir = lights[0].position - vpos;
        if (length( lights[0].attenuation ) != 0.0) {
            FP float dist = length(lightDir);
            att = 1.0 / (lights[0].attenuation.x + lights[0].attenuation.y * dist + lights[0].attenuation.z * dist * dist);
        }
    }

    FP vec3 s = normalize( lightDir );
    FP float diffuse = max( dot( s, n ), 0.0 );

    diffuseColor += att * lights[0].intensity * diffuse * lights[0].color;
}
