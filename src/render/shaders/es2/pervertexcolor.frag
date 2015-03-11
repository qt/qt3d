#define FP highp

uniform FP vec4 lightPosition;
uniform FP vec3 lightIntensity;

varying FP vec3 position;
varying FP vec3 normal;
varying FP vec3 color;

FP vec3 adModel( const FP vec3 pos, const FP vec3 n, const FP vec3 col )
{
    // Calculate the vector from the light to the fragment
    FP vec3 s = normalize( vec3( lightPosition ) - pos );

    // Calculate the diffuse component
    FP float diffuse = max( dot( s, n ), 0.0 );

    // Combine the ambient and diffuse
    return lightIntensity * ( col + col * diffuse );
}

void main()
{
    gl_FragColor = vec4( adModel( position, normalize( normal ), color ), 1.0 );
}
