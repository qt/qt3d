#define FP highp

varying FP vec3 position;
varying FP vec3 normal;
varying FP vec3 color;

#pragma include light.inc.frag

void main()
{
    FP vec3 diffuseColor;
    adModel(position, normal, diffuseColor);
    gl_FragColor = vec4( color + color * diffuseColor, 1.0 );
}
