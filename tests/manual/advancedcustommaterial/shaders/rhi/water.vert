#version 450 core

// These includes will include the default Qt3D uniform buffers
#pragma include phong.inc.frag
#pragma include coordinatesystems.inc

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec4 vertexTangent;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec3 worldNormal;
layout(location = 2) out vec4 worldTangent;
layout(location = 3) out vec2 texCoord;
layout(location = 4) out vec2 movtexCoord;
layout(location = 5) out vec2 multexCoord;
layout(location = 6) out vec2 waveTexCoord;
layout(location = 7) out vec2 skyTexCoord;
layout(location = 8) out vec3 vpos;

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

void main()
{
    // Scale texture coordinates for for fragment shader
    texCoord = vertexTexCoord * texCoordScale;
    movtexCoord = vertexTexCoord * texCoordScale;
    multexCoord = vertexTexCoord * (texCoordScale*0.5);
    waveTexCoord = vertexTexCoord * (texCoordScale * 6);
    skyTexCoord = vertexTexCoord * (texCoordScale * 0.2);

    // Add Animated x and y Offset to SKY, MOV and MUL texCoords
    movtexCoord = vec2(texCoord.x+offsetx,texCoord.y+offsety);
    multexCoord = vec2(texCoord.x-offsetx,texCoord.y+offsety);
    skyTexCoord = vec2(texCoord.x-(offsetx/2),texCoord.y-(offsety/2));

    // Transform position, normal, and tangent to world coords
    worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    worldNormal = normalize(modelNormalMatrix * vertexNormal);
    worldTangent.xyz = normalize(vec3(modelMatrix * vec4(vertexTangent.xyz, 0.0)));
    worldTangent.w = vertexTangent.w;

    // Calculate animated vertex positions

    float sinPos = (vertexPosition.z)+(vertexPosition.x);
    float sinPos2 = (vertexPosition.y/2)+(vertexPosition.z);
    vec3 vertMod = vec3(vertexPosition.x,vertexPosition.y,vertexPosition.z);

    vertMod = vec3(vertMod.x+=sin(vertYpos*2.2-sinPos2)*waveheight,
                   vertMod.y=sin(vertYpos*2.2+sinPos)*waveheight,
                   vertMod.z-=sin(vertYpos*2.2-cos(sinPos2))*waveheight);

    vec3 vertModCom = vec3(vertMod.x+=cos(vertYpos*2.2-cos(sinPos2))*waveheight,
                           vertMod.y=sin(vertYpos*2.2+cos(sinPos))*waveheight,
                           vertMod.z-=cos(vertYpos*2.2-cos(sinPos))*waveheight);


    // Add wave animation only to vertices above world pos.y zero
    if(vertexPosition.y < 0.0){vertModCom = vertexPosition;}
    else{vertModCom = vertModCom;}

    vpos = vertModCom;

    // Calculate vertex position in clip coordinates
    gl_Position = mvp * vec4(vertModCom, 1.0);
}
