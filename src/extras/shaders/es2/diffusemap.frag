#define FP highp

uniform FP vec4 ka;            // Ambient reflectivity
uniform FP vec4 ks;            // Specular reflectivity
uniform FP float shininess;    // Specular shininess factor

uniform FP vec3 eyePosition;

uniform sampler2D diffuseTexture;

varying FP vec3 worldPosition;
varying FP vec3 worldNormal;
varying FP vec2 texCoord;

#pragma include phong.inc.frag

void main()
{
    FP vec4 diffuseTextureColor = texture2D( diffuseTexture, texCoord );
    FP vec3 worldView = normalize(eyePosition - worldPosition);
    gl_FragColor = phongFunction(ka, diffuseTextureColor, ks, shininess, worldPosition, worldView, worldNormal);
}
