#version 150

//layout (location = 0)
in vec3 vertexPosition;
//layout (location = 1)
in vec3 vertexNormal;

out vec3 position;
out vec3 normal;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 mvp;

void main()
{
    normal = normalize( normalMatrix * vertexNormal );
    position = vec3( modelViewMatrix * vec4( vertexPosition, 1.0 ) );

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
