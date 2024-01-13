#version 450

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 worldNormal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 fragColor;

#pragma include phong.inc.frag

void main()
{
    vec3 worldView = normalize(eyePosition - worldPosition);
    fragColor = phongFunction(color, color, vec4(0.0), 0.0, worldPosition, worldView, worldNormal);
}
