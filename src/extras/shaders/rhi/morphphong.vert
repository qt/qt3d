#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexPositionTarget;
layout(location = 3) in vec3 vertexNormalTarget;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec3 worldNormal;

layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelViewMatrix;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 modelViewProjection;
  mat4 inverseModelViewProjectionMatrix;
};

layout(std140, binding = 3) uniform qt3d_morph_uniforms {
  float interpolator;
};

void main()
{
    vec3 morphPos;
    vec3 morphNormal;
    if (interpolator > 0.0) {
        // normalized
        morphPos = mix(vertexPosition, vertexPositionTarget, interpolator);
        morphNormal = normalize(mix(vertexNormal, vertexNormalTarget, interpolator));
    } else {
        // relative
        morphPos = vertexPosition + vertexPositionTarget * abs(interpolator);
        morphNormal = normalize(vertexNormal + vertexNormalTarget * abs(interpolator));
    }

    worldNormal = normalize( modelNormalMatrix * morphNormal );
    worldPosition = vec3( modelMatrix * vec4( morphPos, 1.0 ) );

    gl_Position = modelViewProjection * vec4( morphPos, 1.0 );
}
