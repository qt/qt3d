#version 450

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 worldNormal;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform qt3d_render_view_uniforms {
  mat4 viewMatrix;
  mat4 projectionMatrix;
  mat4 viewProjectionMatrix;
  mat4 inverseViewMatrix;
  mat4 inverseProjectionMatrix;
  mat4 inverseViewProjectionMatrix;
  mat4 viewportMatrix;
  mat4 inverseViewportMatrix;
  vec4 textureTransformMatrix;
  vec3 eyePosition;
  float aspectRatio;
  float gamma;
  float exposure;
  float time;
};

layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelViewMatrix;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 mvp;
  mat4 inverseModelViewProjectionMatrix;
};

layout(std140, binding = 2) uniform qt3d_custom_uniforms {
  vec3 kd;            // Diffuse reflectivity
  vec3 ks;            // Specular reflectivity
  vec3 kblue;         // Cool color
  vec3 kyellow;       // Warm color
  float alpha;        // Fraction of diffuse added to kblue
  float beta;         // Fraction of diffuse added to kyellow
  float shininess;    // Specular shininess factor
};

const int MAX_LIGHTS = 8;
const int TYPE_POINT = 0;
const int TYPE_DIRECTIONAL = 1;
const int TYPE_SPOT = 2;

struct Light {
    int type;
    vec3 position;
    vec3 color;
    float intensity;
    vec3 direction;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float cutOffAngle;
};

layout(std140, binding = 3) uniform qt3d_light_uniforms {
  uniform Light lights[MAX_LIGHTS];
  uniform int lightCount;
  uniform int envLightCount;
};


vec3 goochModel( const in vec3 pos, const in vec3 n )
{
    // Based upon the original Gooch lighting model paper at:
    // http://www.cs.northwestern.edu/~ago820/SIG98/abstract.html

    // Calculate kcool and kwarm from equation (3)
    vec3 kcool = clamp(kblue + alpha * kd, 0.0, 1.0);
    vec3 kwarm = clamp(kyellow + beta * kd, 0.0, 1.0);

    vec3 result = vec3(0.0);
    for (int i = 0; i < lightCount; ++i) {
        // Calculate the vector from the light to the fragment
        vec3 s = normalize( vec3( lights[i].position ) - pos );

        // Calculate the cos theta factor mapped onto the range [0,1]
        float sDotNFactor = ( 1.0 + dot( s, n ) ) / 2.0;

        // Calculate the tone by blending the kcool and kwarm contributions
        // as per equation (2)
        vec3 intensity = mix( kcool, kwarm, sDotNFactor );

        // Calculate the vector from the fragment to the eye position
        vec3 v = normalize( eyePosition - pos );

        // Reflect the light beam using the normal at this fragment
        vec3 r = reflect( -s, n );

        // Calculate the specular component
        float specular = 0.0;
        if ( dot( s, n ) > 0.0 )
            specular = pow( max( dot( r, v ), 0.0 ), shininess );

        // Sum the blended tone and specular highlight
        result += intensity + ks * specular;
    }

    return result;
}

void main()
{
    fragColor = vec4( goochModel( worldPosition, normalize( worldNormal ) ), 1.0 );
}
