#version 150

uniform vec4 lightPosition;
uniform vec3 lightIntensity;
uniform vec3 kd; // Diffuse reflectivity
uniform vec3 ka; // Ambient reflectivity

in vec3 position;
in vec3 normal;

//layout (location = 0)
out vec4 fragColor;

void main()
{
    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( lightPosition ) - position );

    // Calculate the diffuse lighting factor
    vec3 n = normalize( normal );
    float diffuse = max( dot( s, n ), 0.0 );

    // Multiply by incoming light intensity
    fragColor = vec4( lightIntensity * (kd * diffuse + ka), 1.0 );
}
