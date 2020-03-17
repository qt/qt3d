#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

layout(binding = 3) uniform sampler2D diffuseTexture;

void main()
{
    fragColor = texture( diffuseTexture, texCoord );
}
