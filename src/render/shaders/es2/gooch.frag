#define FP highp

// TODO: Replace with a uniform block
uniform FP vec4 lightPosition;
uniform FP vec3 lightIntensity;

// TODO: Replace with a struct
uniform FP vec3 kd;            // Diffuse reflectivity
uniform FP vec3 ks;            // Specular reflectivity
uniform FP vec3 kblue;         // Cool color
uniform FP vec3 kyellow;       // Warm color
uniform FP float alpha;        // Fraction of diffuse added to kblue
uniform FP float beta;         // Fraction of diffuse added to kyellow
uniform FP float shininess;    // Specular shininess factor

varying FP vec3 position;
varying FP vec3 normal;

FP vec3 goochModel( const in FP vec3 pos, const in FP vec3 n )
{
    // Based upon the original Gooch lighting model paper at:
    // http://www.cs.northwestern.edu/~ago820/SIG98/abstract.html

    // Calculate kcool and kwarm from equation (3)
    FP vec3 kcool = clamp(kblue + alpha * kd, 0.0, 1.0);
    FP vec3 kwarm = clamp(kyellow + beta * kd, 0.0, 1.0);

    // Calculate the vector from the light to the fragment
    FP vec3 s = normalize( vec3( lightPosition ) - pos );

    // Calculate the cos theta factor mapped onto the range [0,1]
    FP float sDotNFactor = ( 1.0 + dot( s, n ) ) / 2.0;

    // Calculate the tone by blending the kcool and kwarm contributions
    // as per equation (2)
    FP vec3 intensity = mix( kcool, kwarm, sDotNFactor );

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    FP vec3 v = normalize( -pos );

    // Reflect the light beam using the normal at this fragment
    FP vec3 r = reflect( -s, n );

    // Calculate the specular component
    FP float specular = 0.0;
    if ( dot( s, n ) > 0.0 )
        specular = pow( max( dot( r, v ), 0.0 ), shininess );

    // Sum the blended tone and specular highlight
    return intensity + ks * specular;
}

void main()
{
    gl_FragColor = vec4( goochModel( position, normalize( normal ) ), 1.0 );
}
