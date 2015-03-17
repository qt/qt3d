#version 330

in vec3 vertexPosition;
in vec3 vertexColor;

uniform mat4 u_projection;
uniform mat4 u_model;

out vec3 color;

void main()
{
    // Pass through the color to the fragment shader
    color = vertexColor;

    // Calculate the vertex position
    gl_Position = u_projection * u_model * vec4( vertexPosition, 1.0 );
}
