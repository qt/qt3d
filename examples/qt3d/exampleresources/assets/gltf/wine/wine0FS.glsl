#version 150
//precision highp float;

in vec3 v_normal;
uniform vec4 u_ambient;
in vec2 v_texcoord0;
uniform sampler2D u_diffuse;
out vec4 fragColor;

void main(void) {
    vec3 normal = normalize(v_normal);
    vec4 color = vec4(0., 0., 0., 0.);
    vec4 diffuse = vec4(0., 0., 0., 1.);
    vec4 ambient;
    ambient = u_ambient;
    diffuse = texture(u_diffuse, v_texcoord0);
    diffuse.xyz *= max(dot(normal,vec3(0.,0.,1.)), 0.);
    color.xyz += diffuse.xyz;
    color = vec4(color.rgb * diffuse.a, diffuse.a);
    fragColor = color;
}
