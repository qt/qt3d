attribute highp vec4 qt_Vertex;
attribute highp vec3 qt_Normal;
attribute highp vec4 qt_MultiTexCoord0;
uniform highp mat4 qt_ModelViewProjectionMatrix;
uniform highp mat4 qt_ModelViewMatrix;
uniform highp mat3 qt_NormalMatrix;
varying highp vec4 texCoord0;

// varying values for per-pixel lighting
varying mediump vec4 lightingColor;
varying highp vec3 normal;
varying highp vec3 lightDirection;
varying highp vec3 halfVector;
varying highp vec3 vertexToLight;

// Varying values for per-vertex lighting
varying mediump vec4 litCombinedColor;

// These structs are copied from the QGLShaderProgramEffect documentation
struct qt_MaterialParameters {
    mediump vec4 emission;
    mediump vec4 ambient;
    mediump vec4 diffuse;
    mediump vec4 specular;
    mediump float shininess;
};
uniform qt_MaterialParameters qt_Material;

struct qt_SingleLightParameters {
    mediump vec4 position;
    mediump vec3 spotDirection;
    mediump float spotExponent;
    mediump float spotCutoff;
    mediump float spotCosCutoff;
    mediump float constantAttenuation;
    mediump float linearAttenuation;
    mediump float quadraticAttenuation;
};
uniform qt_SingleLightParameters qt_Light;

// This is a pretty standard lighting function, adapted from QGLLitMaterial.
// The changes are mostly just renaming to improve readablity, but some
// unecessary features are removed too (e.g. support for two-sided materials)

void qLightVertex(vec4 vertex, vec3 normal)
{
    // Start with the material's emissive color and the ambient scene color,
    // which have been combined into the emission parameter.
    vec4 color = qt_Material.emission;
    vec4 scolor = qt_Material.specular;

    // Viewer is at infinity.
    vec3 toEye = vec3(0, 0, 1);

    // Determine the angle between the normal and the light direction.
    vec4 lightPosition = qt_Light.position;
    vec3 lightDirection;
    if (lightPosition.w == 0.0)
        lightDirection = normalize(lightPosition.xyz);
    else
        lightDirection = normalize(lightPosition.xyz - vertex.xyz);
    // Clean up negative values
    float angle = max(dot(normal, lightDirection), 0.0);

    // Calculate the ambient and diffuse light components.
    vec4 adcomponent = qt_Material.ambient + angle * qt_Material.diffuse;

    // Calculate the specular light components, in case they're needed for
    // per-vertex lighting.
    vec4 scomponent;
    if (angle != 0.0) {
        halfVector = normalize(lightDirection + toEye);
        angle = max(dot(normal, halfVector), 0.0);
        float srm = qt_Material.shininess;
        vec4 scm = qt_Material.specular;
        if (srm != 0.0)
            scomponent = pow(angle, srm) * scm;
        else
            scomponent = scm;
    } else {
        scomponent = vec4(0, 0, 0, 0);
    }

    // Apply the spotlight angle and exponent.
    if (qt_Light.spotCutoff != 180.0) {
        vertexToLight = vertex.xyz - lightPosition.xyz;
        float spot = max(dot(normalize(vertexToLight),
                             qt_Light.spotDirection), 0.0);
        if (spot < qt_Light.spotCosCutoff) {
            adcomponent = vec4(0, 0, 0, 0);
            scomponent = vec4(0, 0, 0, 0);
        } else {
            spot = pow(spot, qt_Light.spotExponent);
            adcomponent *= spot;
            scomponent *= spot;
        }
    }

    // Apply attenuation to the colors.
    if (lightPosition.w != 0.0) {
        float attenuation = qt_Light.constantAttenuation;
        float k1 = qt_Light.linearAttenuation;
        float k2 = qt_Light.quadraticAttenuation;
        if (k1 != 0.0 || k2 != 0.0) {
            float len = length(lightPosition.xyz - vertex.xyz);
            attenuation += k1 * len + k2 * len * len;
        }
        color += adcomponent / attenuation;
        scolor += scomponent / attenuation;
    } else {
        color += adcomponent;
        scolor += scomponent;
    }

    // Generate the final output colors to pass to the fragment shader.
    float alpha = qt_Material.diffuse.a;
    // This is the combined diffuse and ambient lighting color
    lightingColor = vec4(clamp(color.rgb, 0.0, 1.0), alpha);
    // Specular lighting and combined color is used for per vertex lighting if
    // per-pixel is too expensive or not required.
    vec4 specularLightingColor = vec4(clamp(scolor.rgb, 0.0, 1.0), 0.0);
    litCombinedColor = clamp(lightingColor + vec4(specularLightingColor.xyz, 0.0), 0.0, 1.0);
}

    void main(void)
    {
        gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
        texCoord0 = qt_MultiTexCoord0;

        // Calculate the vertex and normal to use for lighting calculations.
        highp vec4 vertex = qt_ModelViewMatrix * qt_Vertex;
        normal = normalize(qt_NormalMatrix * qt_Normal);
        qLightVertex(vertex,normal);
}

