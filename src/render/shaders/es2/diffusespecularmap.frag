#define FP highp

uniform FP vec4 lightPosition;
uniform FP vec3 lightIntensity;

// TODO: Replace with a struct
uniform FP vec3 ka;            // Ambient reflectivity
uniform FP float shininess;    // Specular shininess factor

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

varying FP vec3 position;
varying FP vec3 normal;
varying FP vec2 texCoord;

FP vec3 adsModel( const in FP vec3 pos, const in FP vec3 n )
{
    // Calculate the vector from the light to the fragment
    FP vec3 s = normalize( vec3( lightPosition ) - pos );

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    FP vec3 v = normalize( -pos );

    // Reflect the light beam using the normal at this fragment
    FP vec3 r = reflect( -s, n );

    // Calculate the diffuse component
    FP float diffuse = max( dot( s, n ), 0.0 );

    // Calculate the specular component
    FP float specular = 0.0;
    if ( dot( s, n ) > 0.0 )
        specular = ( shininess / ( 8.0 * 3.14 ) ) * pow( max( dot( r, v ), 0.0 ), shininess );

    // Lookup diffuse and specular factors
    FP vec3 diffuseColor = texture2D( diffuseTexture, texCoord ).rgb;
    FP vec3 specularColor = texture2D( specularTexture, texCoord ).rgb;

    // Combine the ambient, diffuse and specular contributions
    return lightIntensity * ( ( ka + diffuse ) * diffuseColor + specular * specularColor );
}

void main()
{
    gl_FragColor = vec4( adsModel( position, normalize( normal ) ), 1.0 );
}
