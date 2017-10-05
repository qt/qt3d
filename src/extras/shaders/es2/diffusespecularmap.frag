#define FP highp

// TODO: Replace with a struct
uniform FP vec4 ka;            // Ambient reflectivity
uniform FP float shininess;    // Specular shininess factor

uniform FP vec3 eyePosition;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

varying FP vec3 worldPosition;
varying FP vec3 worldNormal;
varying FP vec2 texCoord;

#pragma include phong.inc.frag

void main()
{
    FP vec4 diffuseTextureColor = texture2D( diffuseTexture, texCoord );
    FP vec4 specularTextureColor = texture2D( specularTexture, texCoord );
    FP vec3 worldView = normalize(eyePosition - worldPosition);
    gl_FragColor = phongFunction(ka, diffuseTextureColor, specularTextureColor, shininess, worldPosition, worldView, worldNormal);
}
