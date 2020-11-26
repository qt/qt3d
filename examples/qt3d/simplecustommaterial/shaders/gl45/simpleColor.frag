#version 450 core

layout(std140, binding = 2) uniform custom_uniforms {
    vec3 maincolor;
};

layout(location = 0) out vec4 fragColor;

void main()
{
    //output color from material
    fragColor = vec4(maincolor,1.0);
}

