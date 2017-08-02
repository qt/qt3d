#version 150 core

uniform vec4 ka;            // Ambient reflectivity
uniform vec4 ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor

uniform vec3 eyePosition;

uniform sampler2D diffuseTexture;

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;

out vec4 fragColor;

#pragma include light.inc.frag

void main()
{
    vec4 diffuseTextureColor = texture( diffuseTexture, texCoord );
    vec3 worldView = normalize(eyePosition - worldPosition);
    fragColor = phongFunction(ka, diffuseTextureColor, ks, shininess, worldPosition, worldView, worldNormal);
}
