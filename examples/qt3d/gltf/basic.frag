#version 330

in vec3 color;

out vec4 fragColor;

void main()
{
    // The color variable is interpolated to this fragment's position
    // for us by OpenGL.
    fragColor = vec4( color, 1.0 );
}
