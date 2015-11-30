#define FP highp

uniform FP vec4 lightPosition;
uniform FP vec3 lightIntensity;
uniform FP vec3 kd; // Diffuse reflectivity
uniform FP vec3 ka; // Ambient reflectivity

varying FP vec3 position;
varying FP vec3 normal;

void main()
{
    // Calculate the vector from the light to the fragment
    FP vec3 s = normalize( vec3( lightPosition ) - position );

    // Calculate the diffuse lighting factor
    FP vec3 n = normalize( normal );
    FP float diffuse = max( dot( s, n ), 0.0 );

    // Multiply by incoming light intensity
    gl_FragColor = vec4( lightIntensity * (kd * diffuse + ka), 1.0 );
}
