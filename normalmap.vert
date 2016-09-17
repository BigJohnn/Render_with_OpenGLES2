attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoords;

varying vec2 TexCoords;
varying vec3 Normal;
varying vec3 Position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
	Position = vec3(model * vec4(position, 1.0));
    TexCoords = texCoords;
	Normal = normal;
}