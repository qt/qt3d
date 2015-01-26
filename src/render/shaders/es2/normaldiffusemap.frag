precision highp float;

varying vec3 lightDir;
varying vec3 viewDir;
varying vec2 texCoord;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

// TODO: Replace with a uniform block
uniform vec4 lightPosition; // = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec3 lightIntensity; // = vec3(1.0, 1.0, 1.0);

// TODO: Replace with a struct
uniform vec3 ka;            // Ambient reflectivity
uniform vec3 ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor

vec3 adsModel( const vec3 norm, const vec3 diffuseReflect)
{
    // Reflection of light direction about normal
    vec3 r = reflect( -lightDir, norm );

    // Calculate the ambient contribution
    vec3 ambient = lightIntensity * ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( lightDir, norm ), 0.0 );
    vec3 diffuse = lightIntensity * diffuseReflect * sDotN;

    // Sum the ambient and diffuse contributions
    vec3 ambientAndDiff = ambient + diffuse;

    // Calculate the specular highlight contribution
    vec3 spec = vec3( 0.0 );
    if ( sDotN > 0.0 )
        spec = lightIntensity * ks *  pow( max( dot( r, viewDir ), 0.0 ), shininess );
    return ambientAndDiff + spec;
}

void main()
{
    // Sample the textures at the interpolated texCoords
    vec4 diffuseTextureColor = texture2D( diffuseTexture, texCoord );
    vec4 normal = 2.0 * texture2D( normalTexture, texCoord ) - vec4( 1.0 );

    // Combine spec with ambient+diffuse for final fragment color
    gl_FragColor = vec4( adsModel( normalize( normal.xyz ), diffuseTextureColor.xyz), 1.0 );
}
