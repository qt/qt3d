#version 110

uniform sampler2D color;
uniform sampler2D position;
uniform sampler2D normal;
uniform vec2 winSize;

struct PointLight
{
    vec3 position;
    vec3 direction;
    vec4 color;
    float intensity;
};

const int lightCount = 3;
uniform PointLight pointLights[lightCount];

void main()
{
    vec2 texCoord = gl_FragCoord.xy / winSize;
    vec4 col = texture2D(color, texCoord);
    vec3 pos = texture2D(position, texCoord).xyz;
    vec3 norm = texture2D(normal, texCoord).xyz;

    vec4 lightColor;
    for (int i = 0; i < 3; i++) {
        vec3 s = normalize(pointLights[i].position - pos);
        lightColor += pointLights[i].color * (pointLights[i].intensity * max(dot(s, norm), 0.0));
    }
    lightColor /= float(lightCount);
    gl_FragColor = col * lightColor;
}
