#version 150 core

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 position;
out vec3 normal;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

void main()
{
    normal = normalize( modelViewNormal * vertexNormal );
    position = vec3( modelView * vec4( vertexPosition, 1.0 ) );

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
