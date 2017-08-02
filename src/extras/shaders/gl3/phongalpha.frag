#version 150 core

// TODO: Replace with a struct
uniform vec4 ka;            // Ambient reflectivity
uniform vec4 kd;            // Diffuse reflectivity
uniform vec4 ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor
uniform float alpha;

uniform vec3 eyePosition;

in vec3 worldPosition;
in vec3 worldNormal;

out vec4 fragColor;

#pragma include light.inc.frag

void main()
{
    vec3 worldView = normalize(eyePosition - worldPosition);
    fragColor = phongFunction(ka, vec4(kd.rgb, alpha), ks, shininess, worldPosition, worldView, worldNormal);
}
