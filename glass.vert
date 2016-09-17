// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix

attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoords;

varying vec4 TexCoord;
varying vec3 Normal;
varying vec3 ViewVec;
varying vec3 LightVec;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normMatrix;

void main()
{
    vec4 Position  = vec4(position,1.0);
    vec2 TexCoords  = texCoords;

    gl_Position = projection*view*model * Position;
    TexCoord = vec4( TexCoords, ( gl_Position.xy / gl_Position.w ) * 0.5 + 0.5 );

    // calculate light and view vectors.  everything in worldspace.
    vec4 WorldPos = model * Position;
    LightVec    = lightPos - WorldPos.xyz;
    ViewVec     = viewPos   - WorldPos.xyz;

    Normal = normMatrix * normal;
}