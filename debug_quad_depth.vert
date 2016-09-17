attribute vec3 position;
attribute vec2 texCoords;

varying vec2 TexCoords;

void main()
{
    gl_Position = vec4(position, 1.0);
    TexCoords = texCoords;
}
