#version 150
//precision highp float;

in vec3 a_position;
in vec3 a_normal;
out vec3 v_normal;
uniform mat3 u_normalMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat4 u_projectionMatrix;
in vec2 a_texcoord0;
out vec2 v_texcoord0;

void main(void) {
    vec4 pos = u_modelViewMatrix * vec4(a_position,1.0);
    v_normal = normalize(u_normalMatrix * a_normal);
    v_texcoord0 = a_texcoord0;
    gl_Position = u_projectionMatrix * pos;
}
