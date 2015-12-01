#version 150 core

in vec3 position;
in vec3 normal;
in vec3 color;

out vec4 fragColor;

#pragma include light.inc.frag

void main()
{
    vec3 diffuseColor;
    adModel(position, normal, diffuseColor);
    fragColor = vec4( color + color * diffuseColor, 1.0 );
}
