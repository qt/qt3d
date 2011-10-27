attribute highp vec4 qt_Vertex;
attribute highp vec4 qt_MultiTexCoord0;
uniform mediump mat4 qt_ModelViewProjectionMatrix;
varying highp vec4 texCoord;
//            uniform highp float qt_Custom;
uniform int customInt;
uniform vec2 vector;

uniform mat3 matrix3x3;
uniform mat4 matrix4x4;

void main(void)
{
    gl_Position = qt_ModelViewProjectionMatrix * (matrix4x4 * qt_Vertex);
    texCoord = -qt_MultiTexCoord0;
}
