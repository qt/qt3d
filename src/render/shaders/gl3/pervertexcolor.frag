#version 150 core

// TODO: Replace with a uniform block
uniform vec4 lightPosition;
uniform vec3 lightIntensity;

in vec3 position;
in vec3 normal;
in vec3 color;

out vec4 fragColor;

vec3 adModel( const in vec3 pos, const in vec3 n, const in vec3 col )
{
    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( lightPosition ) - pos );

    // Calculate the diffuse component
    float diffuse = max( dot( s, n ), 0.0 );

    // Combine the ambient and diffuse contributions
    return lightIntensity * ( col + col * diffuse );
}

void main()
{
    fragColor = vec4( adModel( position, normalize( normal ), color ), 1.0 );
}

