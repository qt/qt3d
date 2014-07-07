#version 150

//layout (location = 0)
in vec3 vertexPosition;
//layout (location = 1)
in vec3 vertexNormal;

out vec3 position;
out vec3 normal;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 modelViewProjection;

void main()
{
    normal = normalize( modelViewNormal * vertexNormal );
    position = vec3( modelView * vec4( vertexPosition, 1.0 ) );

    gl_Position = modelViewProjection * vec4( vertexPosition, 1.0 );
}
