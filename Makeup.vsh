attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoords;
uniform mat4 modelViewProjection;
varying vec2 texCoordVarying;

void main()
{
    gl_Position = modelViewProjection * vec4(position, 1.0);
    texCoordVarying = texCoords;
}