#version 450 core

// These includes will include the default Qt3D uniform buffers
#pragma include phong.inc.frag
#pragma include coordinatesystems.inc

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 worldNormal;
layout(location = 2) in vec4 worldTangent;
layout(location = 3) in vec2 texCoord;
layout(location = 4) in vec2 waveTexCoord;
layout(location = 5) in vec2 movtexCoord;
layout(location = 6) in vec2 multexCoord;
layout(location = 7)in vec2 skyTexCoord;
layout(location = 8) in vec3 vpos;

layout(std140, binding = auto) uniform wave_uniforms {
    float offsetx;
    float offsety;
    float vertYpos;
    float waveheight;
    float waveRandom;
    float specularity;
    float waveStrenght;
    float normalAmount;
    vec3 specularColor;
    float shininess;
    vec4 ka;
};

layout(binding = auto) uniform sampler2D diffuseTexture;
layout(binding = auto) uniform sampler2D specularTexture;
layout(binding = auto) uniform sampler2D normalTexture;
layout(binding = auto) uniform sampler2D waveTexture;
layout(binding = auto) uniform sampler2D skyTexture;
layout(binding = auto) uniform sampler2D foamTexture;

layout(location = 0) out vec4 fragColor;

void main()
{
    // Move waveTexCoords
    vec2 waveMovCoord = waveTexCoord;
    waveMovCoord.x += offsetx;
    waveMovCoord.y -= offsety;
    vec4 wave = texture(waveTexture, waveMovCoord);

    //Wiggle the newCoord by r and b colors of waveTexture
    vec2 newCoord = texCoord;
    newCoord.x += wave.r * waveStrenght;
    newCoord.y -= wave.b * waveStrenght;

    // Sample the textures at the interpolated texCoords
    // Use default texCoord for diffuse (it does not move on x or y, so it can be used as "ground under the water").
    vec4 diffuseTextureColor = texture(diffuseTexture, texCoord);
    // 2 Animated Layers of specularTexture mixed with the newCoord
    vec4 specularTextureColor = texture( specularTexture, multexCoord+newCoord) + (texture( specularTexture, movtexCoord+newCoord ));
    // 2 Animated Layers of normalTexture mixed with the newCoord
    vec3 tNormal = normalAmount * texture( normalTexture, movtexCoord+newCoord ).rgb - vec3( 1.0 )+(normalAmount * texture( normalTexture, multexCoord+newCoord ).rgb - vec3( 1.0 ));
    // Animated skyTexture layer
    vec4 skycolor = texture(skyTexture, skyTexCoord);
    skycolor = skycolor * 0.4;
    //Animated foamTexture layer
    vec4 foamTextureColor = texture(foamTexture, texCoord);

    mat3 tangentMatrix = calcWorldSpaceToTangentSpaceMatrix(worldNormal, worldTangent);
    mat3 invertTangentMatrix = transpose(tangentMatrix);

    vec3 wNormal = normalize(invertTangentMatrix * tNormal);
    vec3 worldView = normalize(eyePosition - worldPosition);

    vec4 diffuse = vec4(diffuseTextureColor.rgb, vpos.y);
    vec4 specular = vec4(specularTextureColor.a*specularity);
    vec4 outputColor = phongFunction(ka, diffuse, specular, shininess, worldPosition, worldView, wNormal);

    outputColor += vec4(skycolor.rgb, vpos.y);
    outputColor += (foamTextureColor.rgba*vpos.y);

    fragColor = vec4(outputColor.rgb,1.0);
}
