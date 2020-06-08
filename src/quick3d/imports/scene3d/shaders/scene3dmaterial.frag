#version 450

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float visible;
};

layout(binding = 1) uniform sampler2D source;

layout(location = 0) in vec2 qt_TexCoord;
layout(location = 0) out vec4 fragColor;

void main()
{
   if (visible < 0.0)
       discard;
   vec4 p = texture(source, qt_TexCoord);
   float a = qt_Opacity * p.a;
   fragColor = vec4(p.rgb * a, a);
}
