#version 450

layout(location = 0) in vec2 texCoord;
layout(location = 1) in float zValue;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 2) uniform qt3d_custom_uniforms {
  float minAlpha;
  float maxAlpha;
  float textureSize;
  vec4 color;
};
layout(binding = 3) uniform sampler2D distanceFieldTexture;

void main()
{
    // determine the scale of the glyph texture within pixel-space coordinates
    // (that is, how many pixels are drawn for each texel)
    vec2 texelDeltaX = abs(dFdx(texCoord));
    vec2 texelDeltaY = abs(dFdy(texCoord));
    float avgTexelDelta = textureSize * 0.5 * (texelDeltaX.x + texelDeltaX.y + texelDeltaY.x + texelDeltaY.y);
    float texScale = 1.0 / avgTexelDelta;

    // scaled to interval [0.0, 0.15]
    float devScaleMin = 0.00;
    float devScaleMax = 0.15;
    float scaled = (clamp(texScale, devScaleMin, devScaleMax) - devScaleMin) / (devScaleMax - devScaleMin);

    // thickness of glyphs should increase a lot for very small glyphs to make them readable
    float base = 0.5;
    float threshold = base * scaled;
    float range = 0.06 / texScale;

    float minAlpha = threshold - range;
    float maxAlpha = threshold + range;

    float distVal = texture(distanceFieldTexture, texCoord).r;
    fragColor = vec4(color.rgb, color.a * smoothstep(minAlpha, maxAlpha, distVal));
    gl_FragDepth = gl_FragCoord.z - zValue * 0.000001;
}
