#version 150

in vec3 vertexPosition;
in vec3 vertexColor;

out vec3 color;

void main()
{
    // Pass through the color to the fragment shader
    color = vertexColor;

    // Calculate the vertex position
    gl_Position = vec4( vertexPosition, 1.0 );
}
