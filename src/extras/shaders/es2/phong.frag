#define FP highp

uniform FP vec4 ka;            // Ambient reflectivity
uniform FP vec4 kd;            // Diffuse reflectivity
uniform FP vec4 ks;            // Specular reflectivity
uniform FP float shininess;    // Specular shininess factor

uniform FP vec3 eyePosition;

varying FP vec3 worldPosition;
varying FP vec3 worldNormal;

#pragma include phong.inc.frag

void main()
{
    FP vec3 worldView = normalize(eyePosition - worldPosition);
    gl_FragColor = phongFunction(ka, kd, ks, shininess, worldPosition, worldView, worldNormal);
}
