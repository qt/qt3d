const int MAX_LIGHTS = 8;
const int TYPE_POINT = 0;
const int TYPE_DIRECTIONAL = 1;
const int TYPE_SPOT = 2;
struct Light {
    int type;
    FP vec3 position;
    FP vec3 color;
    FP float intensity;
    FP vec3 direction;
    FP vec3 attenuation;
    FP float cutOffAngle;
};
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

void addLightAdsModelNormalMapped(const in FP vec3 vpos,
                                  const in FP vec3 n,
                                  const in FP vec3 eye,
                                  const in FP float shininess,
                                  const in FP mat3 tangentMatrix,
                                  const in Light light,
                                  inout FP vec3 diffuseColor,
                                  inout FP vec3 specularColor)
{
    FP vec3 snormal = normalize( vec3( tangentMatrix[0][2], tangentMatrix[1][2], tangentMatrix[2][2] ) );

    FP vec3 s, ts;
    FP float att = 1.0;
    if ( light.type != TYPE_DIRECTIONAL ) {
        s = light.position - vpos;
        if ( dot(snormal, s) < 0.0 )
            att = 0.0;
        else {
            ts = normalize( tangentMatrix * s );
            if (length( light.attenuation ) != 0.0) {
                FP float dist = length(s);
                att = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist * dist);
            }
            s = normalize( s );
            if ( light.type == TYPE_SPOT ) {
                if ( degrees(acos(dot(-s, normalize(light.direction))) ) > light.cutOffAngle)
                    att = 0.0;
            }
        }
    } else {
        if ( dot(snormal, -light.direction) > 0.0 )
            s = normalize( tangentMatrix * -light.direction );
        else
            att = 0.0;
    }

    FP float diffuse = max( dot( ts, n ), 0.0 );

    FP float specular = 0.0;
    if (diffuse > 0.0 && shininess > 0.0 && att > 0.0) {
        FP vec3 r = reflect( -ts, n );
        FP vec3 v = normalize( tangentMatrix * ( eye - vpos ) );
        FP float normFactor = ( shininess + 2.0 ) / 2.0;
        specular = normFactor * pow( max( dot( r, v ), 0.0 ), shininess );
    }

    diffuseColor += att * light.intensity * diffuse * light.color;
    specularColor += att * light.intensity * specular * light.color;
}

void adsModelNormalMapped(const in FP vec3 vpos, const in FP vec3 vnormal, const in FP vec3 eye, const in FP float shininess,
                          const in FP mat3 tangentMatrix,
                          out FP vec3 diffuseColor, out FP vec3 specularColor)
{
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);
    if (lightCount < 1) return;

    FP vec3 n = normalize( vnormal );
    addLightAdsModelNormalMapped(vpos, n, eye, shininess, tangentMatrix, lights[0], diffuseColor, specularColor);
    if (lightCount < 2) return;

    addLightAdsModelNormalMapped(vpos, n, eye, shininess, tangentMatrix, lights[1], diffuseColor, specularColor);
    if (lightCount < 3) return;

    addLightAdsModelNormalMapped(vpos, n, eye, shininess, tangentMatrix, lights[2], diffuseColor, specularColor);
    if (lightCount < 4) return;

    addLightAdsModelNormalMapped(vpos, n, eye, shininess, tangentMatrix, lights[3], diffuseColor, specularColor);
    if (lightCount < 5) return;

    addLightAdsModelNormalMapped(vpos, n, eye, shininess, tangentMatrix, lights[4], diffuseColor, specularColor);
    if (lightCount < 6) return;

    addLightAdsModelNormalMapped(vpos, n, eye, shininess, tangentMatrix, lights[5], diffuseColor, specularColor);
    if (lightCount < 7) return;

    addLightAdsModelNormalMapped(vpos, n, eye, shininess, tangentMatrix, lights[6], diffuseColor, specularColor);
    if (lightCount < 8) return;

    addLightAdsModelNormalMapped(vpos, n, eye, shininess, tangentMatrix, lights[7], diffuseColor, specularColor);
}

void addLightAdsModel(const in FP vec3 vpos,
                      const in FP vec3 n,
                      const in FP vec3 eye,
                      const in FP float shininess,
                      const in Light light,
                      inout FP vec3 diffuseColor,
                      inout FP vec3 specularColor)
{
    FP vec3 s;

    FP float att = 1.0;
    if ( light.type != TYPE_DIRECTIONAL ) {
        s = light.position - vpos;
        if (length( light.attenuation ) != 0.0) {
            FP float dist = length(s);
            att = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist * dist);
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
        FP vec3 v = normalize( eye - vpos );
        FP float normFactor = ( shininess + 2.0 ) / 2.0;
        specular = normFactor * pow( max( dot( r, v ), 0.0 ), shininess );
    }

    diffuseColor += att * light.intensity * diffuse * light.color;
    specularColor += att * light.intensity * specular * light.color;
}


void adsModel(const in FP vec3 vpos, const in FP vec3 vnormal, const in FP vec3 eye, const in FP float shininess,
              out FP vec3 diffuseColor, out FP vec3 specularColor)
{
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);
    if (lightCount < 1) return;

    FP vec3 n = normalize( vnormal );
    addLightAdsModel(vpos, n, eye, shininess, lights[0], diffuseColor, specularColor);
    if (lightCount < 2) return;

    addLightAdsModel(vpos, n, eye, shininess, lights[1], diffuseColor, specularColor);
    if (lightCount < 3) return;

    addLightAdsModel(vpos, n, eye, shininess, lights[2], diffuseColor, specularColor);
    if (lightCount < 4) return;

    addLightAdsModel(vpos, n, eye, shininess, lights[3], diffuseColor, specularColor);
    if (lightCount < 5) return;

    addLightAdsModel(vpos, n, eye, shininess, lights[4], diffuseColor, specularColor);
    if (lightCount < 6) return;

    addLightAdsModel(vpos, n, eye, shininess, lights[5], diffuseColor, specularColor);
    if (lightCount < 7) return;

    addLightAdsModel(vpos, n, eye, shininess, lights[6], diffuseColor, specularColor);
    if (lightCount < 8) return;

    addLightAdsModel(vpos, n, eye, shininess, lights[7], diffuseColor, specularColor);
}

void addLightAdModel(const in FP vec3 vpos,
                     const in FP vec3 n,
                     const in Light light,
                     inout FP vec3 diffuseColor)
{
    FP vec3 s;
    FP float att = 1.0;
    if ( light.type != TYPE_DIRECTIONAL ) {
        s = light.position - vpos;
        if (length( light.attenuation ) != 0.0) {
            FP float dist = length(s);
            att = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist * dist);
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

    diffuseColor += att * light.intensity * diffuse * light.color;
}

void adModel(const in FP vec3 vpos, const in FP vec3 vnormal, out FP vec3 diffuseColor)
{
    diffuseColor = vec3(0.0);
    if (lightCount < 1) return;

    FP vec3 n = normalize( vnormal );
    addLightAdModel(vpos, n, lights[0], diffuseColor);
    if (lightCount < 2) return;

    addLightAdModel(vpos, n, lights[1], diffuseColor);
    if (lightCount < 3) return;

    addLightAdModel(vpos, n, lights[2], diffuseColor);
    if (lightCount < 4) return;

    addLightAdModel(vpos, n, lights[3], diffuseColor);
    if (lightCount < 5) return;

    addLightAdModel(vpos, n, lights[4], diffuseColor);
    if (lightCount < 6) return;

    addLightAdModel(vpos, n, lights[5], diffuseColor);
    if (lightCount < 7) return;

    addLightAdModel(vpos, n, lights[6], diffuseColor);
    if (lightCount < 8) return;

    addLightAdModel(vpos, n, lights[7], diffuseColor);
}
