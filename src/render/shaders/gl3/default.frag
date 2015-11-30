#version 150 core

uniform vec3 ka;            // Ambient reflectivity
uniform vec3 kd;            // Diffuse reflectivity
uniform vec3 ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor

uniform vec3 eyePosition;

in vec3 position;
in vec3 normal;

out vec4 fragColor;

#pragma include light.inc.frag

void main()
{
    vec3 diffuseColor, specularColor;
    calculateLightColors(position, normal, eyePosition, shininess, diffuseColor, specularColor);
    fragColor = vec4( ka + kd * diffuseColor + ks * specularColor, 1.0 );
}
