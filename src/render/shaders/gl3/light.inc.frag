const int MAX_LIGHTS = 8;
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    vec3 direction;
    vec3 attenuation;
//    float cutOffAngle;
};
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

void adsModel(vec3 vpos, vec3 vnormal, vec3 eye, float shininess, out vec3 diffuseColor, out vec3 specularColor)
{
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);

    vec3 n = normalize( vnormal );

    int i;
    for (i = 0; i < lightCount; ++i) {
        vec3 lightDir = lights[i].direction;
        float att = 1.0;
        if (length( lightDir ) == 0.0) {
            lightDir = lights[i].position - vpos;
            if (length( lights[i].attenuation ) != 0.0) {
                float dist = length(lightDir);
                att = 1.0 / (lights[i].attenuation.x + lights[i].attenuation.y * dist + lights[i].attenuation.z * dist * dist);
            }
        }

        vec3 s = normalize( lightDir );
        float diffuse = max( dot( s, n ), 0.0 );

        float specular = 0.0;
        if (diffuse > 0.0 && shininess > 0.0) {
            vec3 r = reflect( -s, n );
            vec3 v = normalize( eye - vpos );
            specular = ( shininess / ( 8.0 * 3.14 ) ) * pow( max( dot( r, v ), 0.0 ), shininess );
        }

        diffuseColor += att * lights[i].intensity * diffuse * lights[i].color;
        specularColor += specular;
    }
}
