#version 150 core

// TODO: Replace with a struct
uniform vec3 ka;            // Ambient reflectivity
uniform float shininess;    // Specular shininess factor

uniform vec3 eyePosition;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

#pragma include light.inc.frag

void main()
{
    vec3 diffuseTextureColor = texture( diffuseTexture, texCoord ).rgb;
    vec3 specularTextureColor = texture( specularTexture, texCoord ).rgb;

    vec3 diffuseColor, specularColor;
    adsModel(position, normal, eyePosition, shininess, diffuseColor, specularColor);

    fragColor = vec4( diffuseTextureColor * ( ka + diffuseColor ) + specularTextureColor * specularColor, 1.0 );
}
