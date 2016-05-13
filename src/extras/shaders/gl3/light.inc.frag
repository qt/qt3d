const int MAX_LIGHTS = 8;
const int TYPE_POINT = 0;
const int TYPE_DIRECTIONAL = 1;
const int TYPE_SPOT = 2;
struct Light {
    int type;
    vec3 position;
    vec3 color;
    float intensity;
    vec3 direction;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float cutOffAngle;
};
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

void adsModelNormalMapped(const in vec3 vpos, const in vec3 vnormal, const in vec3 eye, const in float shininess,
                          const in mat3 tangentMatrix,
                          out vec3 diffuseColor, out vec3 specularColor)
{
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);

    vec3 n = normalize( vnormal );

    int i;
    vec3 s;
    for (i = 0; i < lightCount; ++i) {
        float att = 1.0;
        if ( lights[i].type != TYPE_DIRECTIONAL ) {
            s = tangentMatrix * ( lights[i].position - vpos );
            if (lights[i].constantAttenuation != 0.0
             || lights[i].linearAttenuation != 0.0
             || lights[i].quadraticAttenuation != 0.0) {
                float dist = length(s);
                att = 1.0 / (lights[i].constantAttenuation + lights[i].linearAttenuation * dist + lights[i].quadraticAttenuation * dist * dist);
            }
            s = normalize( s );
            if ( lights[i].type == TYPE_SPOT ) {
                if ( degrees(acos(dot(-s, normalize(lights[i].direction))) ) > lights[i].cutOffAngle)
                    att = 0.0;
            }
        } else {
            s = normalize( tangentMatrix * -lights[i].direction );
        }

        float diffuse = max( dot( s, n ), 0.0 );

        float specular = 0.0;
        if (diffuse > 0.0 && shininess > 0.0 && att > 0.0) {
            vec3 r = reflect( -s, n );
            vec3 v = normalize( tangentMatrix * ( eye - vpos ) );
            float normFactor = ( shininess + 2.0 ) / 2.0;
            specular = normFactor * pow( max( dot( r, v ), 0.0 ), shininess );
        }

        diffuseColor += att * lights[i].intensity * diffuse * lights[i].color;
        specularColor += att * lights[i].intensity * specular * lights[i].color;
    }
}

void adsModel(const in vec3 vpos, const in vec3 vnormal, const in vec3 eye, const in float shininess,
              out vec3 diffuseColor, out vec3 specularColor)
{
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);

    vec3 n = normalize( vnormal );

    int i;
    vec3 s;
    for (i = 0; i < lightCount; ++i) {
        float att = 1.0;
        if ( lights[i].type != TYPE_DIRECTIONAL ) {
            s = lights[i].position - vpos;
            if (lights[i].constantAttenuation != 0.0
             || lights[i].linearAttenuation != 0.0
             || lights[i].quadraticAttenuation != 0.0) {
                float dist = length(s);
                att = 1.0 / (lights[i].constantAttenuation + lights[i].linearAttenuation * dist + lights[i].quadraticAttenuation * dist * dist);
            }
            s = normalize( s );
            if ( lights[i].type == TYPE_SPOT ) {
                if ( degrees(acos(dot(-s, normalize(lights[i].direction))) ) > lights[i].cutOffAngle)
                    att = 0.0;
            }
        } else {
            s = normalize( -lights[i].direction );
        }

        float diffuse = max( dot( s, n ), 0.0 );

        float specular = 0.0;
        if (diffuse > 0.0 && shininess > 0.0 && att > 0.0) {
            vec3 r = reflect( -s, n );
            vec3 v = normalize( eye - vpos );
            float normFactor = ( shininess + 2.0 ) / 2.0;
            specular = normFactor * pow( max( dot( r, v ), 0.0 ), shininess );
        }

        diffuseColor += att * lights[i].intensity * diffuse * lights[i].color;
        specularColor += att * lights[i].intensity * specular * lights[i].color;
    }
}

void adModel(const in vec3 vpos, const in vec3 vnormal, out vec3 diffuseColor)
{
    diffuseColor = vec3(0.0);

    vec3 n = normalize( vnormal );

    int i;
    vec3 s;
    for (i = 0; i < lightCount; ++i) {
        float att = 1.0;
        if ( lights[i].type != TYPE_DIRECTIONAL ) {
            s = lights[i].position - vpos;
            if (lights[i].constantAttenuation != 0.0
             || lights[i].linearAttenuation != 0.0
             || lights[i].quadraticAttenuation != 0.0) {
                float dist = length(s);
                att = 1.0 / (lights[i].constantAttenuation + lights[i].linearAttenuation * dist + lights[i].quadraticAttenuation * dist * dist);
            }
            s = normalize( s );
            if ( lights[i].type == TYPE_SPOT ) {
                if ( degrees(acos(dot(-s, normalize(lights[i].direction))) ) > lights[i].cutOffAngle)
                    att = 0.0;
            }
        } else {
            s = normalize( -lights[i].direction );
        }

        float diffuse = max( dot( s, n ), 0.0 );

        diffuseColor += att * lights[i].intensity * diffuse * lights[i].color;
    }
}
