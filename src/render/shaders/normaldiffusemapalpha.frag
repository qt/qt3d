#version 150 core

in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

// TODO: Replace with a uniform block
uniform vec4 lightPosition;
uniform vec3 lightIntensity;

// TODO: Replace with a struct
uniform vec3 ka;            // Ambient reflectivity
uniform vec3 ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor

out vec4 fragColor;

void adsModel( const in vec3 norm, const in vec3 diffuseReflect, out vec3 ambientAndDiff, out vec3 spec )
{
    // Reflection of light direction about normal
    vec3 r = reflect( -lightDir, norm );

    // Calculate the ambient contribution
    vec3 ambient = lightIntensity * ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( lightDir, norm ), 0.0 );
    vec3 diffuse = lightIntensity * diffuseReflect * sDotN;

    // Sum the ambient and diffuse contributions
    ambientAndDiff = ambient + diffuse;

    // Calculate the specular highlight contribution
    spec = vec3( 0.0 );
    if ( sDotN > 0.0 )
        spec = lightIntensity * ks *  pow( max( dot( r, viewDir ), 0.0 ), shininess );
}

void main()
{
    // Sample the textures at the interpolated texCoords
    vec4 diffuseTextureColor = texture( diffuseTexture, texCoord );
    vec4 normal = 2.0 * texture( normalTexture, texCoord ) - vec4( 1.0 );

    // Calculate the lighting model, keeping the specular component separate
    vec3 ambientAndDiff, spec;
    adsModel( normalize( normal.xyz ), diffuseTextureColor.xyz, ambientAndDiff, spec );

    // Combine spec with ambient+diffuse for final fragment color
    // Use the alpha from the diffuse texture (for alpha to coverage)
    fragColor = vec4( ambientAndDiff + spec, diffuseTextureColor.a );
}
