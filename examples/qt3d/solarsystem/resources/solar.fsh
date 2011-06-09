uniform sampler2D qt_Texture0;
uniform sampler2D qt_Texture1;
varying mediump vec4 lightingColor;
varying highp vec4 texCoord0;
uniform mediump float glowFactor;

void main(void)
{
    vec4 unlitColor0 = texture2D(qt_Texture0, texCoord0.st);
    vec4 unlitColor1 = texture2D(qt_Texture1, texCoord0.st);

    // interpolate between the two textures
    vec4 unlitColor = mix(unlitColor0, unlitColor1, glowFactor);

    // Apply lighting
    vec4 litColor = lightingColor * unlitColor;
    // Normally, we'd just paint this lit colour like so:
//    gl_FragColor = litColor;

    // However, we want a cool glow effect, because the sun is generating
    // it's own light.

    // Multiplying the color by itself will washing out the darker colours,
    // leaving the brighter colours brighter by comparison
    vec4 powerColor = unlitColor * unlitColor;
    powerColor = powerColor * powerColor ;

    // Then combine the higher contrast color and the lit color
    gl_FragColor= mix(litColor, powerColor, glowFactor);
}
