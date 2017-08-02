#define FP highp

varying FP vec3 worldPosition;
varying FP vec3 worldNormal;
varying FP vec4 worldTangent;
varying FP vec2 texCoord;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

// TODO: Replace with a struct
uniform FP vec3 ka;            // Ambient reflectivity
uniform FP float shininess;    // Specular shininess factor

uniform FP vec3 eyePosition;

#pragma include light.inc.frag
#pragma include coordinatesystems.inc

void main()
{
    // Sample the textures at the interpolated texCoords
    FP vec4 diffuseTextureColor = texture2D( diffuseTexture, texCoord );
    FP vec4 specularTextureColor = texture2D( specularTexture, texCoord );
    FP vec3 tNormal = 2.0 * texture2D( normalTexture, texCoord ).rgb - vec3( 1.0 );

    FP mat3 tangentMatrix = calcWorldSpaceToTangentSpaceMatrix(worldNormal, worldTangent);
    FP mat3 invertTangentMatrix = transpose(tangentMatrix);

    FP vec3 wNormal = normalize(invertTangentMatrix * tNormal);

    // Calculate the lighting model, keeping the specular component separate
    FP vec3 diffuseColor, specularColor;
    adsModel(worldPosition, wNormal, eyePosition, shininess, diffuseColor, specularColor);

    // Combine spec with ambient+diffuse for final fragment color
    gl_FragColor = vec4( ka + diffuseTextureColor.rgb * diffuseColor + specularTextureColor.rgb * specularColor, 1.0 );
}
