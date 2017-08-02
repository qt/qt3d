#version 150 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 color;

out vec4 fragColor;

uniform vec3 eyePosition;

#pragma include light.inc.frag

void main()
{
    vec3 diffuseColor;
    vec3 specularColor;
    adsModel(worldPosition, worldNormal, eyePosition, 0.0, diffuseColor, specularColor);
    fragColor = vec4( color + color * diffuseColor, 1.0 );
}
