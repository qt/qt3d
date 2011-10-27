varying highp vec4 texCoord;
uniform sampler2D qt_Texture0;
uniform lowp vec4 customColor;
uniform bool customBoolean;

void main(void)
{
    mediump vec4 textureColor =
            texture2D(qt_Texture0, texCoord.st);
    gl_FragColor = clamp(vec4(
            customColor.rgb * (1.0 - textureColor.a) +
            textureColor.rgb, 1.0), 0.0, 1.0);
}
