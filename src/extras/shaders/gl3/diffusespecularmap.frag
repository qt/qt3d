#version 150 core

// TODO: Replace with a struct
uniform vec4 ka;            // Ambient reflectivity
uniform float shininess;    // Specular shininess factor

uniform vec3 eyePosition;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;

out vec4 fragColor;

#pragma include phong.inc.frag

void main()
{
    vec4 diffuseTextureColor = texture( diffuseTexture, texCoord );
    vec4 specularTextureColor = texture( specularTexture, texCoord );
    vec3 worldView = normalize(eyePosition - worldPosition);
    fragColor = phongFunction(ka, diffuseTextureColor, specularTextureColor, shininess, worldPosition, worldView, worldNormal);
}
