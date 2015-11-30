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

vec3 lightColor(vec3 vpos, vec3 vnormal)
{
    vec3 n = normalize( vnormal );
    vec3 c = vec3( 0.0 );
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
        c += att * (diffuse * lights[i].color) * lights[i].intensity;
    }
    return c;
}
