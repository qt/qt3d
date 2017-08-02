#define FP highp

varying FP vec3 worldPosition;
varying FP vec3 worldNormal;
varying FP vec3 color;

uniform FP vec3 eyePosition;

#pragma include light.inc.frag

void main()
{
    FP vec3 diffuseColor;
    FP vec3 specularColor;
    adsModel(worldPosition, worldNormal, eyePosition, 0.0, diffuseColor, specularColor);
    gl_FragColor = vec4( color + color * diffuseColor, 1.0 );
}
