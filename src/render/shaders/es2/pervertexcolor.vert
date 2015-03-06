attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec3 vertexColor;

varying vec3 position;
varying vec3 normal;
varying vec3 color;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

void main()
{
    normal = normalize( modelViewNormal * vertexNormal );
    position = vec3( modelView * vec4( vertexPosition, 1.0 ) );
    color = vertexColor;

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
