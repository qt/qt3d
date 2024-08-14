#version 450

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 0) out vec2 qt_TexCoord;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float visible;
};

void main()
{
    qt_TexCoord = vertexTexCoord;
    gl_Position = qt_Matrix * vertexPosition;
}
